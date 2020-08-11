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
        for (auto raw : dependencies) {
            if (raw != dependencies[0]) {
                std::cout << " ";
            }
            VariableValue *value = nullptr;
            bool isDouble = false;
            VarType type;
            if (raw->operatorType == LITERAL) {
                type = raw->type;
                value = static_cast<VariableValue *>(raw->value);
                isDouble = raw->isDouble;
            } else if (raw->operatorType == VARIABLE) {
                auto *var = static_cast<Variable *>(raw->value);
                if(raw->isList) {
                    value = skx::Variable::extractValueFromList(var, raw->indexDescriptor);

                } else {
                 value = var->getValue();
                    type = var->type;
                    isDouble = raw->isDouble;
                }

            }
            if (value == nullptr) return nullptr;
            std::cout << value->getStringValue();
        }
        std::cout << "\n";
        return nullptr;
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
            for (int32_t i = 0; i < localLoopTarget; ++i) {
                exec->execute(rootItem);
                if (exec->stopLoop) break;
                i = num->intValue;
                num->intValue++;
            }
        }
        delete exec;
        return nullptr;
    }
}