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

bool skx::Comparison::execute(skx::Context *context) {
    if (type == ASSIGN || type == SUBTRACT || type == ADD || type == MULTIPLY || type == DIVIDE)
        return false;
    if (source->type == STRING && type != EQUAL && type != NOT_EQUAL)
        return false;
    VariableValue *sourceValue;
    VariableValue *targetValue;
    Variable *sourceVar = nullptr;
    Variable *targetVar = nullptr;
    if (source->operatorType == LITERAL) {
        sourceValue = static_cast<VariableValue *>(source->value);
    } else if (source->operatorType == VARIABLE) {
        auto *out = static_cast<Variable *>(source->value);
        sourceValue = out->getValue();
        sourceVar = out;
    } else {
        sourceValue = nullptr;
    }
    if (target->operatorType == LITERAL) {
        targetValue = static_cast<VariableValue *>(target->value);
    } else if (target->operatorType == VARIABLE) {
        auto *out = static_cast<Variable *>(target->value);
        targetValue = out->getValue();
        targetVar = out;
    } else {
        targetValue = nullptr;
    }
    if (targetValue == nullptr || sourceValue == nullptr) return false;
    if (sourceValue->type != targetValue->type && target->type != UNDEFINED) {
    return type == NOT_EQUAL;
    }

    //this is a bit special case
    if (target->type == UNDEFINED) {
        if (type != EQUAL && type != NOT_EQUAL) return false;
        return type == EQUAL ? sourceVar->type == UNDEFINED : sourceVar->type != UNDEFINED;
    }

    switch (type) {
        case EQUAL:
            return sourceValue->isEqual(targetValue);
        case NOT_EQUAL:
            return !sourceValue->isEqual(targetValue);
        case BIGGER:
            return sourceValue->isBigger(targetValue);
        case SMALLER:
            return sourceValue->isSmaller(targetValue);
        case BIGGER_OR_EQUAL:
            return sourceValue->isBiggerOrEquals(targetValue);
        case SMALLER_OR_EQUAL:
            return sourceValue->isSmallerOrEquals(targetValue);
        default:
            break;

    }

    return false;
}

skx::Comparison::~Comparison() {
    delete source;
    delete target;
}

bool skx::Assigment::execute(skx::Context *context) {
    if (target->operatorType == LITERAL) return false; // something went wrong here
    auto *targetVar = static_cast<Variable *>(target->value);
    if (targetVar->state == SPOILED) {
        return false;
    }
    if (source->type == UNDEFINED && source->operatorType != EXECUTION) {
        targetVar->type = UNDEFINED;
        targetVar->setValue(nullptr);
        return true;
    }
  //  if (source->type != target->type && source->operatorType != EXECUTION) return false;
    VariableValue *sourceValue;

    if (source->operatorType == LITERAL) {
        sourceValue = static_cast<VariableValue *>(source->value);
    } else if (source->operatorType == EXECUTION) {
        auto *execTarget = static_cast<Execution *>(source->value);
        OperatorPart *executionResult = execTarget->execute(context);
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
    }
    if(sourceValue == nullptr)
        return false;
    if(targetVar->getValue() == nullptr || sourceValue->type != targetVar->type) {
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
        }
        Variable::createVarValue(sourceValue->type, targetVar, sourceValue->type == NUMBER && dynamic_cast<TNumber*>(sourceValue)->isDouble);
    }
    if( source->type == POINTER) {
        targetVar->setValue(sourceValue);
        targetVar->type = source->type;
        if(sourceValue->varRef != nullptr) {
            targetVar->customTypeName = sourceValue->varRef->customTypeName;
        }
    }
    switch (type) {
        case ASSIGN:
            return targetVar->getValue()->assign(sourceValue);
        case ADD:
            return targetVar->getValue()->add(sourceValue);
        case SUBTRACT:
            return targetVar->getValue()->subtract(sourceValue);
        case MULTIPLY:
            return targetVar->getValue()->multiply(sourceValue);
        case DIVIDE:
            return targetVar->getValue()->divide(sourceValue);
        default:
            break;
    }
    return true;
}

skx::Assigment::~Assigment() {
    delete source;
    delete target;
    source = nullptr;
    target = nullptr;
}

skx::OperatorPart::OperatorPart(skx::OperatorType operatorType, skx::VarType type, void *value, bool isDouble)
        : operatorType(operatorType), type(type), value(value), isDouble(isDouble) {}


skx::OperatorPart::~OperatorPart() {
    if (value != nullptr) {
        if (operatorType == LITERAL) {
            switch (type) {
                case STRING: {
                    delete static_cast<TString *>(value);
                    break;
                }
                case NUMBER: {
                    delete static_cast<TNumber *>(value);
                    break;
                }
                case BOOLEAN: {
                    delete static_cast<TBoolean *>(value);
                    break;
                }
                default:
                    break;
            }
        } else if (operatorType == EXECUTION) {
            delete static_cast<Execution*>(value);
        }
        value = nullptr;
    }
}

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
