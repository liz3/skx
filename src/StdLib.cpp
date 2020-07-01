//
// Created by liz3 on 01/07/2020.
//

#include <iostream>
#include "../include/stdlib.h"
#include "../include/utils.h"
#include "../include/Function.h"


namespace skx {
    Variable* Print::execute(Context *target) {
        for (auto current : dependencies) {
            Variable *sourceVar;
            if (current->type == STATIC || current->type == GLOBAL) {
                sourceVar = skx::Utils::searchRecursive(current->name, target->global);
            } else {
                sourceVar = skx::Utils::searchRecursive(current->name, target);
            }
            if(!sourceVar) {
                return nullptr;
            }
           std::string* val = static_cast<std::string*>(sourceVar->value);
           std::cout << (*val) << "\n";
        }
        return nullptr;
    }

    Variable *FunctionInvoker::execute(Context *target) {
        std::vector<Variable*> invokeVars;
        for (auto current : dependencies) {
            Variable *sourceVar;
            if (current->type == STATIC || current->type == GLOBAL) {
                sourceVar = skx::Utils::searchRecursive(current->name, target->global);
            } else {
                sourceVar = skx::Utils::searchRecursive(current->name, target);
            }
            if(!sourceVar) {
                return nullptr;
            }
            invokeVars.push_back(sourceVar);
        }
        auto result = function->run(invokeVars);
        return result;
    }
}