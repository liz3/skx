//
// Created by liz3 on 16/08/2020.
//

#include "../../include/api/MapEffects.h"
#include "../../include/types/TMap.h"
#include "../../include/types/TBoolean.h"

skx::OperatorPart *skx::MapEffects::execute(skx::Context *target) {
    if(execType == CONTAINS) {
        if(dependencies.size() != 2) return nullptr;
        Variable* var = static_cast<Variable *>(dependencies[0]->value);
        if(var->type != MAP) return nullptr;
        TMap* map = dynamic_cast<TMap*>(var->getValue());
        std::string search = "";
        OperatorPart* p = dependencies[1];
        if(p->operatorType == LITERAL){
            search= static_cast<VariableValue*>(p->value)->getStringValue();
        } else if(p->operatorType == VARIABLE) {
            Variable* v = static_cast<Variable*>(p->value);
            search = v->getValue()->getStringValue();
        }
        return new OperatorPart(LITERAL, BOOLEAN, new TBoolean(map->keyMap.find(search) != map->keyMap.end()), false);
    }
    return nullptr;
}
