//
// Created by liz3 on 29/06/2020.
//

#include "../include/Function.h"

skx::OperatorPart *skx::Function::run(std::vector<OperatorPart *> execVars, Context *callingContext) {
    if (execVars.size() != targetParams.size())
        return nullptr; //param len mismatch
    OperatorPart *returnValue = nullptr;
    //setup stack
    for (uint32_t i = 0; i < targetParams.size(); ++i) {
        auto currentTarget = targetParams[i];
        auto targetVar = execVars[i];
        if (targetVar->operatorType == VARIABLE) {
            Variable *tVar = static_cast<Variable *>(targetVar->value);
            Variable *funcVar = functionItem->ctx->vars[currentTarget->name];
            skx::Utils::copyVariableValue(tVar, funcVar);
        } else if (targetVar->operatorType == LITERAL) {
            Variable *funcVar = functionItem->ctx->vars[currentTarget->name];
            funcVar->type = targetVar->type;
            funcVar->setValue(static_cast<VariableValue*>( targetVar->value));
        }
    }
    ReturnOpWithCtx *returnVal = nullptr;
    CompileItem *item = functionItem;
    isLoop = false;
    stopLoop = false;
    //Execute func
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

skx::ReturnOpWithCtx *skx::Function::walk(skx::CompileItem *item) {
    if(!isLoop && item->isLoop) isLoop = true;
    if (item->returner != nullptr) {
        auto *r = new ReturnOpWithCtx();
        r->descriptor = item->returner;
        r->ctx = item->ctx;
        isLoop = false;
        return r;
    }
    if(item->isElse && lastFailed && lastFailLevel == item->level) {
        for (auto child : item->children) {
            walk(child);
        }
        lastFailed = false;
        return nullptr;
    }
    if(item->isBreak && isLoop) {
        if(!stopLoop)
            stopLoop = true;
        isLoop = false;
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
            if (failed) return nullptr;
        }

        for (auto child : item->children) {
            skx::Utils::updateVarState(child->ctx, RUNTIME_CR);
            auto out = walk(child);
            skx::Utils::updateVarState(child->ctx, SPOILED);
            if (out != nullptr) return out;
        }
        return nullptr;
    }
    if (item->triggers.empty() && item->assignments.size() > 0 && item->comparisons.size() == 0) {
        for (uint32_t i = 0; i < item->assignments.size(); ++i) {
            auto current = item->assignments[i];
            bool failed = !current->execute(item->ctx);
            if (failed) return nullptr;
        }
        //assignments should not have children
    }
    if (!item->executions.empty()) {
        for (uint32_t i = 0; i < item->executions.size(); ++i) {
            auto current = item->executions[i];
            current->execute(item->ctx);
        }
    }
    return nullptr;
}

skx::Function::~Function() {
    for (auto & targetParam : targetParams) {
        delete targetParam;
    }
    targetParams.clear();
    delete functionItem;
}
