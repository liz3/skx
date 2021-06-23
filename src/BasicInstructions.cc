//
// Created by liz3 on 01/07/2020.
//

#include <iostream>
#include "../include/BasicInstructions.h"
#include "../include/utils.h"
#include "../include/Executor.h"

#include "../include/Function.h"
#include "../include/types/TNumber.h"
#include "../include/types/TMap.h"
#include "../include/types/TString.h"

namespace skx {
OperatorPart *Print::execute(Context *target) {
  std::string result = "";
  for (auto root : dependencies) {
    OperatorPart* raw = nullptr;
    if(root->operatorType == EXECUTION) {
      raw = static_cast<Execution*>(root->value)->execute(target);
    } else {
      raw = root;
    }
    if (root != dependencies[0]) {
      result += " ";
    }

    VariableValue *value = nullptr;
    bool isDouble = false;
    if (raw->operatorType == LITERAL) {

      value = static_cast<VariableValue *>(raw->value);
      isDouble = raw->isDouble;
    } else if (raw->operatorType == VARIABLE) {
      auto *var = static_cast<Variable *>(raw->value);
      if(raw->isList) {
        value = skx::Variable::extractValueFromList(var, raw->indexDescriptor);

      } else {
        value = var->getValue();

        isDouble = raw->isDouble;
      }

    }
    if (value == nullptr) return nullptr;
    result += value->getStringValue();
  }
  result += "\n";
  auto* g = target->global;
  if(g->printFunc != nullptr)
    (g->printFunc)(result, 1);
  else
    std::cout << result;
  return nullptr;
}

OperatorPart *StringTemplate::execute(Context* target) {

  std::string rendered = "";
  uint16_t depsCount = 0;
  for(size_t i = 0; i < baseParts.size(); i++)  {
    auto current = baseParts[i];
    if(current.type == 0) {
      rendered += current.val;
    }else {
      auto *var = static_cast<Variable *>(dependencies[depsCount]->value);
      depsCount++;
      if(!var) continue;
      auto* value = var->getValue();
      if(!value) continue;
      rendered += value->getStringValue();

    }
  }

  return new OperatorPart(LITERAL, STRING, new TString(rendered), false);
}

OperatorPart *FunctionInvoker::execute(Context *target) {
  auto result = function->run(dependencies, target);

  return result;
}

OperatorPart *Loop::execute(Context *target) {
  Executor *exec = new Executor();
  if(isIterator) {
    if(!iteratorVar || iteratorVar->type != MAP) {
      delete exec;
      return nullptr;
    }
    TMap* map = dynamic_cast<TMap *>(iteratorVar->getValue());
    loopCounter->type = STRING;
    TString* loopIndexVal = new TString();
    loopCounter->setValue(loopIndexVal);
    for (auto& entry : map->value) {
      loopIndexVal->value = entry.key;
      VariableValue* v = entry.value;
      iteratorValue->type = v->type;
      if(iteratorValue->getValue() != v)
        iteratorValue->setValue(v);
      if(v->type == POINTER)
        iteratorValue->customTypeName = v->customTypeName;
      exec->execute(rootItem);
      if (exec->stopLoop) break;
    }
    delete exec;
    return nullptr;
  }
  if (hasCondition) {
    if (comparison != nullptr) {
      while (comparison->execute(target)) {
        exec->execute(rootItem);
      }
    }
  } else {
    TNumber* num = nullptr;
    if (loopCounter) {
      num = new TNumber(0);
      loopCounter->setValue(num);
    }
    int32_t localLoopTarget = loopTargetVar != nullptr
                              ? dynamic_cast<TNumber *>(loopTargetVar->getValue())->intValue
                              : loopTarget;
    if(loopCounter)
      loopCounter->state = RUNTIME_CR;
    for (int32_t i = 0; i < localLoopTarget; ++i) {
      exec->execute(rootItem);
      if (exec->stopLoop) break;
      i = num->intValue;
      num->intValue++;
    }
    if(loopCounter)
      loopCounter->state = SPOILED;

  }
  delete exec;
  return nullptr;
}
}
