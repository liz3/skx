//
// Created by liz3 on 30/06/2020.
//

#include <iostream>
#include "../include/Instruction.h"
#include "../include/utils.h"
#include "../include/Array.h"

bool skx::Comparison::execute(skx::Context *context) {
    if (source->type != target->type && target->type != UNDEFINED) return false;
    if (type == ASSIGN || type == SUBTRACT || type == ADD || type == MULTIPLY || type == DIVIDE)
        return false;
    if (source->type == STRING && type != EQUAL && type != NOT_EQUAL) return false;
    void *sourceValue;
    void *targetValue;
    Variable *sourceVar = nullptr;
    Variable *targetVar = nullptr;
    if (source->operatorType == LITERAL) {
        sourceValue = source->value;
    } else if (source->operatorType == VARIABLE) {
        auto *out = static_cast<Variable *>(source->value);
        sourceValue = out->value;
        sourceVar = out;
    } else {
        sourceValue = nullptr;
    }
    if (target->operatorType == LITERAL) {
        targetValue = target->value;
    } else if (target->operatorType == VARIABLE) {
        auto *out = static_cast<Variable *>(target->value);
        targetValue = out->value;
        targetVar = out;
    } else {
        targetValue = nullptr;
    }
    if (targetValue == nullptr || sourceValue == nullptr) return false;

    //this is a bit special case
    if (target->type == UNDEFINED) {
        if (type != EQUAL && type != NOT_EQUAL) return false;
        return type == EQUAL ? sourceVar->type == UNDEFINED : sourceVar->type != UNDEFINED;
    }

    //normal type checking comparisons
    if (target->type == BOOLEAN) {
        if (type != EQUAL && type != NOT_EQUAL) return false;
        bool *a = static_cast<bool *>(sourceValue);
        bool *b = static_cast<bool *>(targetValue);
        return type == EQUAL ? *a == *b : *a != *b;
    }
    if (target->type == STRING) {
        if (type != EQUAL && type != NOT_EQUAL) return false;
        auto *a = static_cast<std::string *>(sourceValue);
        auto *b = static_cast<std::string *>(targetValue);
        return type == EQUAL ? *a == *b : *a != *b;
    }

    if (target->type == CHARACTER) {
        auto *a = static_cast<char *>(sourceValue);
        auto *b = static_cast<char *>(targetValue);
        if (type == EQUAL) return *a == *b;
        if (type == NOT_EQUAL) return *a != *b;
        if (type == SMALLER) return *a < *b;
        if (type == BIGGER) return *a > *b;
        if (type == SMALLER_OR_EQUAL) return *a <= *b;
        if (type == BIGGER_OR_EQUAL) return *a >= *b;

    }
    if (target->type == NUMBER) {
        //ik this is ugly, il think about a better approach later. but auto isnt smart enough to change type based on ternnity operator
        if(source->isDouble && target->isDouble) {
            auto *a = static_cast<double *>(sourceValue);
            auto *b = static_cast<double *>(targetValue);
            if (type == EQUAL) return *a == *b;
            if (type == NOT_EQUAL) return *a != *b;
            if (type == SMALLER) return *a < *b;
            if (type == BIGGER) return *a > *b;
            if (type == SMALLER_OR_EQUAL) return *a <= *b;
            if (type == BIGGER_OR_EQUAL) return *a >= *b;
        }
        if(source->isDouble) {
            auto *a = static_cast<double *>(sourceValue);
            auto *b = static_cast<int32_t *>(targetValue);
            if (type == EQUAL) return *a == *b;
            if (type == NOT_EQUAL) return *a != *b;
            if (type == SMALLER) return *a < *b;
            if (type == BIGGER) return *a > *b;
            if (type == SMALLER_OR_EQUAL) return *a <= *b;
            if (type == BIGGER_OR_EQUAL) return *a >= *b;
        }
        if(target->isDouble) {
            auto *a = static_cast<int32_t *>(sourceValue);
            auto *b = static_cast<double *>(targetValue);
            if (type == EQUAL) return *a == *b;
            if (type == NOT_EQUAL) return *a != *b;
            if (type == SMALLER) return *a < *b;
            if (type == BIGGER) return *a > *b;
            if (type == SMALLER_OR_EQUAL) return *a <= *b;
            if (type == BIGGER_OR_EQUAL) return *a >= *b;
        }
        auto *a = static_cast<int32_t *>(sourceValue);
        auto *b = static_cast<int32_t *>(targetValue);
        if (type == EQUAL) return *a == *b;
        if (type == NOT_EQUAL) return *a != *b;
        if (type == SMALLER) return *a < *b;
        if (type == BIGGER) return *a > *b;
        if (type == SMALLER_OR_EQUAL) return *a <= *b;
        if (type == BIGGER_OR_EQUAL) return *a >= *b;
    }

    if (target->type == ARRAY) {
        auto *a = static_cast<Array *>(sourceValue);
        auto *b = static_cast<Array *>(targetValue);
        if (type == EQUAL) return a->entries.size() == b->entries.size();
        if (type == NOT_EQUAL) return a->entries.size() != b->entries.size();
        if (type == SMALLER) return a->entries.size() < b->entries.size();
        if (type == BIGGER) return a->entries.size() > b->entries.size();
        if (type == SMALLER_OR_EQUAL) return a->entries.size() <= b->entries.size();
        if (type == BIGGER_OR_EQUAL) return a->entries.size() >= b->entries.size();
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
    if(targetVar->state == SPOILED) {
        return false;
    }
    if (source->type == UNDEFINED && source->operatorType != EXECUTION) {
        targetVar->type = UNDEFINED;
        targetVar->value = nullptr;
        return true;
    }
    if (source->type != target->type && source->operatorType != EXECUTION) return false;
    void *sourceValue;
    if (source->operatorType == LITERAL) {
        sourceValue = source->value;
    } else if (source->operatorType == EXECUTION) {
        auto* execTarget = static_cast<Execution*>(source->value);
        OperatorPart* executionResult = execTarget->execute(context);
        if(executionResult == nullptr) {
            sourceValue = nullptr;
        } else {
            if(executionResult->operatorType == VARIABLE) {
                sourceValue = static_cast<Variable*>(executionResult->value)->value;
                source->type = static_cast<Variable*>(executionResult->value)->type;
            } else {
                if(executionResult->operatorType == LITERAL) {
                    source->type = executionResult->type;
                }
                sourceValue = executionResult->value;
            }
        }

    }else {
        auto *sourceVar = static_cast<Variable *>(source->value);
        if(sourceVar->state == SPOILED) {
            return false;
        }
        sourceValue = sourceVar->value;
    }
    if (source->type == BOOLEAN && type == INVERT) {
        bool *val = static_cast<bool * >(sourceValue);
        targetVar->value = new bool(!(*val));
        return true;
    }
    if(type == INVERT) return false;
    if(source->type == STRING && type == ADD) {
        auto * b = static_cast<std::string*>(sourceValue);
        auto * a = static_cast<std::string*>(targetVar->value);
        auto* out = new std::string (*a + *b);
        delete a;
        targetVar->value = out;
        return true;
    }
    if(source->type == NUMBER && type != ASSIGN) {
        if(!source->isDouble && !target->isDouble) {
            auto * b = static_cast<int32_t *>(sourceValue);
            auto * a = static_cast<int32_t*>(targetVar->value);
            int32_t * out;
            switch (type) {
                case SUBTRACT:
                    out = new int32_t (*b - *a);
                    break;
                case ADD:
                    out = new int32_t (*b + *a);
                    break;
                case MULTIPLY:
                    out = new int32_t ((*b) * (*a));
                    break;
                case DIVIDE:
                    out = new int32_t ((*b) / (*a));
                    break;
                default:
                    out = nullptr;
            }
            if(out == nullptr) return false;
            delete a;
            targetVar->value = out;
        }
        if(source->isDouble && !target->isDouble) {
            auto * b = static_cast<double *>(sourceValue);
            auto * a = static_cast<int32_t*>(targetVar->value);
            int32_t * out;
            switch (type) {
                case SUBTRACT:
                    out = new int32_t (*b - *a);
                    break;
                case ADD:
                    out = new int32_t (*b + *a);
                    break;
                case MULTIPLY:
                    out = new int32_t ((*b) * (*a));
                    break;
                case DIVIDE:
                    out = new int32_t ((*b) / (*a));
                    break;
                default:
                    out = nullptr;
            }
            if(out == nullptr) return false;
            delete a;
            targetVar->value = out;
        }
        if(!source->isDouble && target->isDouble) {
            auto * b = static_cast<int32_t *>(sourceValue);
            auto * a = static_cast<double *>(targetVar->value);
            double * out;
            switch (type) {
                case SUBTRACT:
                    out = new double (*b - *a);
                    break;
                case ADD:
                    out = new double (*b + *a);
                    break;
                case MULTIPLY:
                    out = new double ((*b) * (*a));
                    break;
                case DIVIDE:
                    out = new double ((*b) / (*a));
                    break;
                default:
                    out = nullptr;
            }
            if(out == nullptr) return false;
            delete a;
            targetVar->value = out;
        }
        if(target->isDouble && source->isDouble) {
            auto * b = static_cast<double *>(sourceValue);
            auto * a = static_cast<double *>(targetVar->value);
            double * out;
            switch (type) {
                case SUBTRACT:
                    out = new double (*b - *a);
                    break;
                case ADD:
                    out = new double (*b + *a);
                    break;
                case MULTIPLY:
                    out = new double ((*b) * (*a));
                    break;
                case DIVIDE:
                    out = new double ((*b) / (*a));
                    break;
                default:
                    out = nullptr;
            }
            if(out == nullptr) return false;
            delete a;
            targetVar->value = out;
        }
        return true;
    }

    targetVar->type = source->type;
    targetVar->value = sourceValue;


    return true;
}

skx::Assigment::~Assigment() {

    delete source;
    delete target;
}

skx::OperatorPart::OperatorPart(skx::OperatorType operatorType, skx::VarType type, void *value, bool isDouble)
        : operatorType(operatorType), type(type), value(value), isDouble(isDouble) {}

skx::OperatorPart::~OperatorPart() {
    if(value == nullptr) return;
    switch (operatorType) {
        case EXECUTION: {
            delete (static_cast<Execution*>(value));
            break;
        }
        case LITERAL: {
            if(type == STRING) {
                std::string * target = static_cast<std::string*>(value);
                delete target;

            }
            if(type == NUMBER) {
                if (isDouble) {
                    auto *val = static_cast<double *>(value);
                    delete val;
                } else {
                    auto *val = static_cast<int32_t *>(value);
                    delete val;
                }

            }
            break;
        }
        default:
            break;
    }
}

//skx::OperatorPart::~OperatorPart() {
////    if(operatorType == LITERAL) {
////       if(value != nullptr) {
////           switch (type) {
////               case STRING: {
////                   delete static_cast<std::string*>(value);
////               }
////               default:
////                   break;
////           }
////       }
////    }
//    value = nullptr;
//}

skx::OperatorPart *skx::Execution::execute(skx::Context *target) {
    return nullptr;
}

skx::Execution::~Execution() {
    for (auto & dependencie : dependencies) {

        delete dependencie;
    }
    dependencies.clear();
}
