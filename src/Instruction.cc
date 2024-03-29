//
// Created by liz3 on 30/06/2020.
//

#include <iostream>
#include "../include/Instruction.h"
#include "../include/utils.h"
#include "../include/types/TString.h"
#include "../include/types/TNumber.h"
#include "../include/types/TBoolean.h"
#include "../include/types/TCharacter.h"
#include "../include/types/TArray.h"
#include "../include/types/TMap.h"

bool skx::Comparison::execute(skx::Context *context) {
  if (type == ASSIGN || type == SUBTRACT || type == ADD || type == MULTIPLY || type == DIVIDE)
    return false;
  if (source->type == STRING && target->type == STRING && type != EQUAL && type != NOT_EQUAL) {
    return false;
  }
  VariableValue *sourceValue = nullptr;
  VariableValue *targetValue = nullptr;
  OperatorPart* sourcePrt = nullptr;
  OperatorPart* targetPrt = nullptr;
  bool freeSource = false;
  bool freeTarget = false;
  Variable *sourceVar = nullptr;
  Variable *targetVar = nullptr;


  if (source->operatorType == LITERAL) {
    sourceValue = static_cast<VariableValue *>(source->value);
  } else if (source->operatorType == VARIABLE) {
    auto *out = static_cast<Variable *>(source->value);
    if(source->isList) {
      if(out->type != MAP) {
        sourceValue = nullptr;
      } else {
        auto* xx = skx::Variable::extractValueFromList(out, source->indexDescriptor);
        if(xx) sourceValue = xx;
      }
    } else {
      sourceValue = out->getValue();
      sourceVar = out;
    }
  } else if(source->operatorType == EXECUTION) {
    auto* result = static_cast<Execution*>(source->value)->execute(context);
    if(result == nullptr) {
      sourceValue = nullptr;
    } else {
      freeSource = result->free;
      sourcePrt = result;
      if(result->operatorType == LITERAL) {
        sourceValue = static_cast<VariableValue*>(result->value);
      } else if(result->operatorType == VARIABLE) {
        Variable* tVar = static_cast<Variable*>(result->value);
        sourceValue = tVar->getValue();
      }
    }
  }
  if (target->operatorType == LITERAL) {
    targetValue = static_cast<VariableValue *>(target->value);
  } else if (target->operatorType == VARIABLE) {
    auto *out = static_cast<Variable *>(target->value);
    if(target->isList) {
      if(out->type != MAP) {
        targetValue = nullptr;
      } else {
        auto* xx = skx::Variable::extractValueFromList(out, target->indexDescriptor);
        if(xx) targetValue = xx;
      }
    } else {
      targetValue = out->getValue();
      targetVar = out;
    }

  } else if(target->operatorType == EXECUTION) {
    auto* result = static_cast<Execution*>(target->value)->execute(context);
    if(result == nullptr) {
      targetValue = nullptr;
    } else {
      freeTarget = result->free;
      targetPrt = result;
      if(result->operatorType == LITERAL) {
        targetValue = static_cast<VariableValue*>(result->value);
      } else if(result->operatorType == VARIABLE) {
        Variable* tVar = static_cast<Variable*>(result->value);
        targetValue = tVar->getValue();
      }
    }
  }
  if (targetValue == nullptr || sourceValue == nullptr) return false;
  if (sourceValue->type != targetValue->type && target->type != UNDEFINED) {
    return type == NOT_EQUAL;
  }

  //this is a bit special case
  if (target->type == UNDEFINED && target->operatorType != EXECUTION && targetValue == nullptr) {
    if (type != EQUAL && type != NOT_EQUAL) return false;
    return type == EQUAL ? sourceVar->type == UNDEFINED : sourceVar->type != UNDEFINED;
  }
  bool res = false;
  switch (type) {
  case EQUAL:
    res = sourceValue->isEqual(targetValue);
    break;
  case NOT_EQUAL:
    res = !sourceValue->isEqual(targetValue);
    break;
  case BIGGER:
    res = sourceValue->isBigger(targetValue);
    break;
  case SMALLER:
    res = sourceValue->isSmaller(targetValue);
    break;
  case BIGGER_OR_EQUAL:
    res = sourceValue->isBiggerOrEquals(targetValue);
    break;
  case SMALLER_OR_EQUAL:
    res = sourceValue->isSmallerOrEquals(targetValue);
    break;
  default:
    break;

  }
  if(freeSource) {
    delete sourcePrt;
  }
  if(freeTarget) {
    delete targetPrt;
  }
  return inverted ? !res : res;
}

skx::Comparison::~Comparison() {
  if(source != nullptr) {
    delete source;
    source = nullptr;
  }

  if(target != nullptr) {

    delete target;
    target = nullptr;  }
}

bool skx::Assigment::execute(skx::Context *context) {
  if (target->operatorType == LITERAL) return false; // something went wrong here
  auto *targetVar = static_cast<Variable *>(target->value);
  if (targetVar->state == SPOILED) {
    return false;
  }
  if (source->type == UNDEFINED && source->operatorType != EXECUTION) {
    bool f = false;
    // try to find var type at runtime
    if(source->operatorType == VARIABLE) {
      auto * srcVar = static_cast<Variable*>(source->value);
      if (srcVar->type != UNDEFINED) {
        source->type = srcVar->type;
        f = true;
      }
    }
    if(!f) {
      targetVar->type = UNDEFINED;
      targetVar->setValue(nullptr);
      return true;
    }
  }
  //  if (source->type != target->type && source->operatorType != EXECUTION) return false;
  VariableValue *sourceValue = nullptr;
  OperatorPart* freeAfterAssign = nullptr;
  if (source->operatorType == LITERAL) {
    sourceValue = static_cast<VariableValue *>(source->value);
  } else if (source->operatorType == EXECUTION) {
    auto *execTarget = static_cast<Execution *>(source->value);
    OperatorPart *executionResult = execTarget->execute(context);
    if(execTarget->name == "native::callinterface") // im not proud of this but its the most effective way to avoid duplicated code, maybe needs refactor later
      freeAfterAssign = executionResult;
    if (executionResult == nullptr) {
      sourceValue = nullptr;
    } else {
      if (executionResult->operatorType == VARIABLE) {
        sourceValue = static_cast<Variable *>(executionResult->value)->getValue();
        source->type = static_cast<Variable *>(executionResult->value)->type;
      } else {
        if (executionResult->operatorType == LITERAL) {
          source->type = executionResult->type;
        }
        sourceValue = static_cast<VariableValue *>( executionResult->value);
      }
    }

  } else {
    auto *sourceVar = static_cast<Variable *>(source->value);
    if (sourceVar->state == SPOILED) {
      return false;
    }
    sourceValue = sourceVar->getValue();
    source->type = sourceValue->type;
  }
  if(target->isList) {
    Variable* s = static_cast<Variable*>(target->value);
    if(s->getValue() == nullptr || s->type != MAP) {
      s->setValue(new TMap());
      s->type = MAP;
    }
    TMap* map = dynamic_cast<TMap*>(s->getValue());
    std::string name = target->indexDescriptor->getValue()->getStringValue();
    auto* newVal = sourceValue->copyValue();
    if(sourceValue->varRef != nullptr && sourceValue->type == POINTER) {
      newVal->customTypeName = sourceValue->varRef->customTypeName;
    }

    if(type == ASSIGN) {
      if(map->keyMap.find(name) != map->keyMap.end()) {
        for (uint32_t i = 0; i < map->value.size(); ++i) {
          if(map->value[i].value == map->keyMap[name]) {
            map->value.erase(map->value.begin() + i);
            break;
          }
        }
        map->keyMap.erase(name);
      }
      map->keyMap[name] = newVal;
      map->value.push_back(MapEntry{name, newVal});
    } else {
      if(map->keyMap.find(name) == map->keyMap.end())
        return false;
      auto * srcMapVal = map->keyMap[name];
      switch (type) {
      case ADD:
        return srcMapVal->add(sourceValue);
      case SUBTRACT:
        return srcMapVal->subtract(sourceValue);
      case MULTIPLY:
        return  srcMapVal->multiply(sourceValue);
      case DIVIDE:
        return srcMapVal->divide(sourceValue);
      default:
        break;
      }
    }
    if(freeAfterAssign != nullptr)
      delete freeAfterAssign;
    return true;
  }

  if(sourceValue == nullptr)
    return false;
  if((targetVar->getValue() == nullptr || sourceValue->type != targetVar->type) && type == ASSIGN) {
    if(targetVar->getValue() != nullptr) {
      switch (targetVar->type) {
      case STRING: {
        auto *val = dynamic_cast<TString*>(targetVar->getValue());
        delete val;
        break;
      }
      case NUMBER: {
        auto *val = dynamic_cast<TNumber*>(targetVar->getValue());
        delete val;
        break;
      }
      case CHARACTER: {
        auto *val = dynamic_cast<TCharacter*>(targetVar->getValue());
        delete val;
        break;
      }
      case BOOLEAN: {
        auto *val = dynamic_cast<TBoolean*>(targetVar->getValue());
        delete val;
        break;
      }
      default:
        break;
      }
      targetVar->setValue(nullptr);
    }
    //special cases
    if(sourceValue->type == MAP && type == ASSIGN) {
      targetVar->type = MAP;
      targetVar->setValue(sourceValue->copyValue());

    if(freeAfterAssign != nullptr)
      delete freeAfterAssign;

      return true;
    }


    Variable::createVarValue(sourceValue->type, targetVar, sourceValue->type == NUMBER && dynamic_cast<TNumber*>(sourceValue)->isDouble);
  }
  if(source->type == POINTER) {
    targetVar->setValue(sourceValue);
    targetVar->type = source->type;
    if(sourceValue->varRef != nullptr) {
      targetVar->customTypeName = sourceValue->varRef->customTypeName;
    }
  }
  bool res = false;
  switch (type) {
  case ASSIGN:
    res = targetVar->getValue()->assign(sourceValue);
    break;
  case ADD:
    res =  targetVar->getValue()->add(sourceValue);
    break;
  case SUBTRACT:
    res= targetVar->getValue()->subtract(sourceValue);
    break;
  case MULTIPLY:
    res= targetVar->getValue()->multiply(sourceValue);
    break;
  case DIVIDE:
    res = targetVar->getValue()->divide(sourceValue);
    break;
  default:
    break;
  }
    if(freeAfterAssign != nullptr)
      delete freeAfterAssign;

  return  res;
}

skx::Assigment::~Assigment() {
  delete source;
  delete target;
  source = nullptr;
  target = nullptr;
}

skx::OperatorPart::OperatorPart(skx::OperatorType operatorType, skx::VarType type, void *value, bool isDouble, bool free)
  : operatorType(operatorType), type(type), value(value), isDouble(isDouble), free(free) {}


skx::OperatorPart::~OperatorPart() {
  if(operatorType == LITERAL) {
    auto* val =  static_cast<VariableValue*>(value);
    if(val != nullptr && val->varRef == nullptr)
      delete val;
  } else if (operatorType == EXECUTION) {
    auto* val = static_cast<Execution*>(value);
    if(val != nullptr)
      delete val;
  }
  value = nullptr;
}

skx::OperatorPart::OperatorPart(skx::OperatorType operatorType, skx::VarType type, skx::Variable *value,
                                skx::Variable *indexDescriptor, bool isDouble, bool free)
  : operatorType(operatorType), type(type), value(value), isDouble(isDouble), free(free), isList(true), indexDescriptor(indexDescriptor)  {

}

skx::Comparison::Comparison() : combineType(START) {}

skx::Comparison::Comparison(ConditionCombineType type) : combineType(type) {}

skx::OperatorPart *skx::Execution::execute(skx::Context *target) {
  return nullptr;
}

skx::Execution::~Execution() {
  for (auto &dependencie : dependencies) {
    delete dependencie;
  }
  dependencies.clear();
}

skx::Trigger::~Trigger() {

}
