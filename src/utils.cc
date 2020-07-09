//
// Created by liz3 on 29/06/2020.
//

#include "../include/utils.h"
#include "../include/Array.h"

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
            target->value = new bool(*(static_cast<bool *>(source->value)));
            break;
        case CHARACTER:
            target->value = new char(*(static_cast<char *>(source->value)));
            break;
        case NUMBER:
            if (source->isDouble) {
                target->value = new double(*(static_cast<double *>(source->value)));
            } else {
                target->value = new int32_t(*(static_cast<int32_t *>(source->value)));
            }
            break;
        case UNDEFINED:
            target->value = nullptr;
            break;
        case STRING:
            target->value = new std::string(*(static_cast<std::string *>(source->value)));
            break;
        case ARRAY:
            //THIS IS NOT A DEEP COPY
            auto *array = static_cast<Array *>(source->value);
            auto *copy = new Array();
            copy->entries = std::vector<Variable *>(array->entries);
            target->value = copy;
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
