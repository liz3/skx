//
// Created by liz3 on 29/06/2020.
//

#include "../include/Variable.h"
#include "../include/utils.h"


void skx::Variable::createVarFromOption(std::string item, skx::Context *targetContext, bool isStatic) {
    auto start = item.find_first_of(':') + 1;
    std::string name = skx::Utils::trim(item.substr(0, start - 1));
    std::string *value = new std::string(skx::Utils::trim(item.substr(start, item.length() - 1)));
    skx::Variable *variable = new Variable();
    if (*value == "true") {
        variable->type = BOOLEAN;
        variable->value = new bool(true);
        delete value;

    } else if (*value == "false") {
        variable->type = BOOLEAN;
        variable->value = new bool(false);
        delete value;
    } else {
        variable->type = STRING;
        variable->value = value;
    }
    variable->ctx = targetContext;
    variable->name = name;
    variable->accessType = STATIC;
    targetContext->vars[name] = variable;
}

skx::VariableDescriptor *skx::Variable::extractNameSafe(std::string in) {
    auto *descriptor = new VariableDescriptor();
    descriptor->isFromContext = in[0] == '%' && in[in.length()-1] == '%';
    auto trim = in.substr(1, in.length() - 2);
    if (trim.rfind('@', 0) == 0) {
        descriptor->type = STATIC;
        trim = trim.substr(1);
    } else if (trim.rfind('_', 0) == 0) {
        descriptor->type = CONTEXT;
        trim = trim.substr(1);
    } else {
        descriptor->type = descriptor->isFromContext ? CONTEXT : GLOBAL;
    }
    descriptor->name = trim;
    return descriptor;
}

skx::Variable::~Variable() {
    if (value != nullptr && !created) {
        switch (type) {
            case STRING: {
                auto *val = static_cast<std::string *>(value);

                delete val;
                break;
            }
            case NUMBER: {
                if (isDouble) {
                    auto *val = static_cast<double *>(value);
                    delete val;
                } else {
                    auto *val = static_cast<int32_t *>(value);
                    delete val;
                }
                break;
            }
            case CHARACTER: {
                auto *val = static_cast<char *>(value);
                delete val;
                break;
            }
            case BOOLEAN: {
                auto *val = static_cast<bool *>(value);
                delete val;
                break;
            }
            default:
                break;
        }
    }
    value = nullptr;
}
