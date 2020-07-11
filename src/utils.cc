//
// Created by liz3 on 29/06/2020.
//

#include "../include/utils.h"
#include "../include/types/TBoolean.h"
#include "../include/types/TCharacter.h"
#include "../include/types/TNumber.h"
#include "../include/types/TString.h"
#include "../include/types/TArray.h"

std::vector<std::string> skx::Utils::split(std::string base, std::string delimiter) {
    std::vector<std::string> final;
    size_t pos = 0;
    std::string token;
    while ((pos = base.find(delimiter)) != std::string::npos) {
        token = base.substr(0, pos);
        final.push_back(token);
        base.erase(0, pos + delimiter.length());
    }
    final.push_back(base);
    return final;
}

skx::Variable *skx::Utils::searchRecursive(std::string what, Context *ctx) {
    Context *current = ctx;
    while (true) {
        auto out = current->vars.find(what);
        if (out != current->vars.end()) return out->second;
        if (current->parent == nullptr) break;
        current = current->parent;
    }
    return nullptr;
}

void skx::Utils::copyVariableValue(skx::Variable *source, skx::Variable *target) {
    switch (source->type) {
        case BOOLEAN:
            target->setValue( new TBoolean(dynamic_cast<TBoolean*>(source->getValue())->value));
            break;
        case CHARACTER:
            target->setValue(new TCharacter(dynamic_cast<TCharacter*>(source->getValue())->value));
            break;
        case NUMBER:
            if (source->isDouble) {
                target->setValue(new TNumber(dynamic_cast<TNumber*>(source->getValue())->doubleValue));
            } else {
                target->setValue(new TNumber(dynamic_cast<TNumber*>(source->getValue())->intValue));
            }
            break;
        case UNDEFINED:
            target->setValue(nullptr);
            break;
        case POINTER:
            target->setValue(source->getValue());
            break;
        case STRING:
            target->setValue(new TString(dynamic_cast<TString*>(source->getValue())->value));
            break;
        case ARRAY:
            //THIS IS NOT A DEEP COPY
            TArray* sourceArray = dynamic_cast<TArray*>(source->getValue());
            TArray* out = new TArray();
            out->assign(sourceArray);
            target->setValue(out);
            break;

    }
    target->type = source->type;
}

void skx::Utils::updateVarState(skx::Context *ctx, skx::VarState state) {
    for (auto const& item : ctx->vars) {
       item.second->state = state;
    }
}
skx::Variable *skx::Utils::searchVar(skx::VariableDescriptor *descriptor, skx::Context *ctx) {
    if(descriptor->type == GLOBAL || descriptor->type == STATIC) {
        return Utils::searchRecursive(descriptor->name, ctx->global);
    }
    return Utils::searchRecursive(descriptor->name, ctx);

}
