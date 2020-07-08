//
// Created by liz3 on 29/06/2020.
//

#include "../include/Variable.h"
#include "../include/utils.h"


void skx::Variable::createVarFromOption(std::string item, skx::Context *targetContext, bool isStatic) {
    auto start = item.find_first_of(':') + 1;
    std::string name = skx::Utils::trim(item.substr(0, start -1));
    std::string *value = new std::string(skx::Utils::trim(item.substr(start, item.length() -1)));
    skx::Variable* variable = new Variable();
    if(*value == "true") {
        variable->type = BOOLEAN;
        variable->value = new bool (true);
    } else if(*value == "false") {
        variable->type = BOOLEAN;
        variable->value = new bool (false);
    } else {
        variable->type = STRING;
        variable->value = value;
    }
    variable->ctx = targetContext;
    variable->name = name;
    variable->accessType =STATIC;
    targetContext->vars[name] = variable;
}

skx::ValueDescriptor* skx::Variable::extractNameSafe(std::string in) {
    auto trim = in.substr(1, in.length() - 2);
    auto* descriptor = new ValueDescriptor();
    if(trim.rfind('@', 0) == 0){
        descriptor->type = STATIC;
        trim = trim.substr(1);
    } else if(trim.rfind('_', 0) == 0) {
        descriptor->type = CONTEXT;
        trim = trim.substr(1);
    } else {
        descriptor->type = GLOBAL;
    }
    descriptor->name = trim;
    return descriptor;
}
