//
// Created by liz3 on 01/07/2020.
//

#include <iostream>
#include "../include/stdlib.h"
#include "../include/utils.h"
#include "../include/Executor.h"

#include "../include/Function.h"


namespace skx {
    OperatorPart* Print::execute(Context *target) {
        for (auto raw : dependencies) {
            if(raw != dependencies[0]) {
                std::cout << " ";
            }
            void* value = nullptr;
            bool isDouble = false;
            VarType type;
            if(raw->operatorType == LITERAL) {
                type = raw->type;
                value = raw->value;
                isDouble = raw->isDouble;
            }else if (raw->operatorType == VARIABLE) {
                auto* var = static_cast<Variable*>(raw->value);
                value = var->value;
                type = var->type;
                isDouble = raw->isDouble;

            }
            if(value == nullptr) return nullptr;
            if(type == STRING) {
                auto* val = static_cast<std::string*>(value);
                std::cout << (*val);
            } else if(type == BOOLEAN) {
                bool * v = static_cast<bool *>(value);
                std::string val((*v) ? "true" : "false");
                std::cout << (val);
            } else if (type == NUMBER) {
                if(isDouble) {
                    auto * val = static_cast<double *>(value);
                    std::cout << (*val);
                } else {
                    auto * val = static_cast<int32_t *>(value);
                    std::cout << (*val);
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
        Executor* exec = new Executor();
        if(hasCondition) {
           if(comparison != nullptr) {
               while (comparison->execute(target)) {
                   exec->execute(rootItem);
               }
           }
        } else {
            if(loopCounter) {
                delete static_cast<int32_t*>(loopCounter->value);
                loopCounter->value = new int32_t (0);
            }
            int32_t localLoopTarget = loopTargetVar != nullptr
                    ? (*static_cast<int32_t*>(loopTargetVar->value))
                    : loopTarget;
            for (int32_t i = 0; i < localLoopTarget; ++i) {
                exec->execute(rootItem);
                if(exec->stopLoop) break;
                i = (*static_cast<int32_t*>(loopCounter->value));
                (*static_cast<int32_t*>(loopCounter->value))++;
            }
        }
        delete exec;
        return nullptr;
    }
}