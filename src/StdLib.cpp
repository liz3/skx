//
// Created by liz3 on 01/07/2020.
//

#include <iostream>
#include "../include/stdlib.h"
#include "../include/utils.h"
#include "../include/Function.h"


namespace skx {
    OperatorPart* Print::execute(Context *target) {
        for (auto raw : dependencies) {
            void* value = nullptr;
            VarType type;
            if(raw->operatorType == LITERAL) {
                type = raw->type;
                value = raw->value;
            } else if(raw->operatorType == DESCRIPTOR) {
                VariableDescriptor* current = static_cast<VariableDescriptor*>(raw->value);
                Variable *sourceVar;
                if (current->type == STATIC || current->type == GLOBAL) {
                    sourceVar = skx::Utils::searchRecursive(current->name, target->global);
                } else {
                    sourceVar = skx::Utils::searchRecursive(current->name, target);
                }
                if(!sourceVar) {
                    return nullptr;
                }
                value = sourceVar->value;
                type = sourceVar->type;
            }
            if(value == nullptr) return nullptr;
            if(type == STRING) {
                std::string* val = static_cast<std::string*>(value);
                std::cout << (*val) << "\n";
            } else if(type == BOOLEAN) {
                bool * v = static_cast<bool *>(value);
                std::string val((*v) ? "true" : "false");
                std::cout << (val) << "\n";
            }
        }
        return nullptr;
    }

    OperatorPart *FunctionInvoker::execute(Context *target) {
        auto result = function->run(dependencies, target);

        return result;
    }
}