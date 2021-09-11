//
// Created by liz3 on 30/06/2020.
//

#include "../include/Executor.h"
#include "../include/utils.h"

#include <iostream>


void skx::Executor::executeStandalone(skx::CompileItem *item) {
  auto *exec = new Executor();
  exec->isLoop = item->isLoop;
  skx::Utils::updateVarState(item->ctx, RUNTIME_CR);
  exec->root = item;
  for (auto current : item->children) {
    exec->walk(current);
  }
  skx::Utils::updateVarState(item->ctx, SPOILED);
  delete exec;
}

skx::OperatorPart* skx::Executor::executeFunction(skx::CompileItem* item) {
  isFunction=true;
  OperatorPart* returnValue = nullptr;
  ReturnOpWithCtx* returnVal = nullptr;
skx::Utils::updateVarState(item->ctx, RUNTIME_CR);
  for (auto current : item->children) {
    returnVal = walk(current);
    if (returnVal != nullptr) break;
  }
  skx::Utils::updateVarState(item->ctx, SPOILED);
  if (returnVal != nullptr) {
    returnValue = returnVal->descriptor->targetReturnItem;
  }

  delete returnVal;
  return returnValue;

}
skx::ReturnOpWithCtx* skx::Executor::walk(skx::CompileItem *item) {
  skx::Utils::updateVarState(item->ctx, RUNTIME_CR);
  if(isFunction && item->returner != nullptr) {
    auto* r = new ReturnOpWithCtx();
    r->descriptor = item->returner;
    r->ctx = item->ctx;
    isLoop = false;
    return r;
  }
  if(item->isElse && lastFailed && lastFailLevel == item->level) {
    for (auto child : item->children) {
      auto* res = walk(child);
      if(isFunction && res != nullptr) {
        lastFailed = false;
        return res;
      }
    }
    lastFailed = false;
    return nullptr;
  }
  if(item->isBreak && isLoop) {
    if(!stopLoop)
      stopLoop = true;
    return nullptr;
  } else if(stopLoop) {
    return nullptr;
  }
  if(item->level > lastFailLevel && lastFailed) {
    lastFailed = false;
  }
  if (item->triggers.empty() && item->assignments.size() == 0 && item->comparisons.size() > 0) {
    for (uint32_t i = 0; i < item->comparisons.size(); ++i) {
      auto current = item->comparisons[i];
      bool failed = !current->execute(item->ctx);
      if (failed) {
        if (i < item->comparisons.size() -1 && item->comparisons[i+1]->combineType == OR)
          continue;
        lastFailed = true;
        lastFailLevel = item->level;
        skx::Utils::updateVarState(item->ctx, SPOILED);
        return nullptr;
      } else if (i < item->comparisons.size() -1 && item->comparisons[i+1]->combineType == OR) {
        break;
      }
    }
    for (auto child : item->children) {
      if(stopLoop) return nullptr;

      auto * res = walk(child);
      if(isFunction && res != nullptr)
        return res;
    }

    skx::Utils::updateVarState(item->ctx, SPOILED);
    return nullptr;
  }
  if (item->triggers.empty() && item->assignments.size() > 0 && item->comparisons.size() == 0) {
    for (uint32_t i = 0; i < item->assignments.size(); ++i) {
      auto current = item->assignments[i];
      bool failed = !current->execute(item->ctx);
      if (failed) {
        skx::Utils::updateVarState(item->ctx, SPOILED);
        return nullptr;
      }
    }
    //assignments should not have children
  }
  if(!item->executions.empty()) {
    for (uint32_t i = 0; i < item->executions.size(); ++i) {
      auto current = item->executions[i];
      auto *res = current->execute(item->ctx);
      if(isFunction && res != nullptr) {
        // TODO make this cleaner somehow, right now id say this is rather hacky
        auto result = new ReturnOpWithCtx();
        result->ctx = item->ctx;
        result->descriptor = new ReturnOperation();
        result->descriptor->targetReturnItem = res;
        return result;
      }
      }
  }
  skx::Utils::updateVarState(item->ctx, SPOILED);
  return nullptr;
}

skx::OperatorPart* skx::Executor::execute(skx::CompileItem *item) {
  isLoop = item->isLoop;
  skx::Utils::updateVarState(item->ctx, RUNTIME_CR);
  root = item;
  for (auto current : item->children) {
    if(stopLoop) break;
    auto res = walk(current);
    if(isFunction && res != nullptr) {
      auto* toReturn = res->descriptor->targetReturnItem;
      delete res;
      return toReturn;
    }
  }
  skx::Utils::updateVarState(item->ctx, SPOILED);
  return nullptr;
}
