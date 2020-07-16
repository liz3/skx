//
// Created by liz3 on 01/07/2020.
//

#include <iostream>
#include "../include/BasicInstructions.h"
#include "../include/utils.h"
#include "../include/Executor.h"

#include "../include/Function.h"
#include "../include/types/TNumber.h"
#include "../include/types/TString.h"
#include "../include/types/TBoolean.h"


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
                value = var->getValue();
                type = var->type;
                isDouble = raw->isDouble;

            }
            if (value == nullptr) return nullptr;
            if (type == STRING) {
                auto val = dynamic_cast<TString *>(value)->value;
                std::cout << val;
            } else if (type == BOOLEAN) {
                auto v = dynamic_cast<TBoolean *>(value)->value;
                std::string val(v ? "true" : "false");
                std::cout << (val);
            } else if (type == NUMBER) {
                auto *val = dynamic_cast<TNumber *>(value);
                if (isDouble) {
                    std::cout << val->doubleValue;
                } else {
                    std::cout << val->intValue;
                }
            }
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