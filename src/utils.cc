//
// Created by liz3 on 29/06/2020.
//

#include "../include/utils.h"
#include "../include/types/TArray.h"
#include "../include/EventInfo.h"

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
    target->setValue(source->getValue()->copyValue());
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

std::string skx::Utils::getEventClassFromExpression(std::string content) {
    for(auto& entry : eventMap) {
        for (int i = 0; i < entry.len; ++i) {
            if(entry.matches[i] == content) {
                return std::string(entry.javaClass);
            }
        }
    }
    return "";
}

std::string skx::Utils::depListToString(std::vector<OperatorPart *> deps) {
    std::string finalMessage;
    for(auto & dep : deps) {
        if(finalMessage.length() != 0) finalMessage += ' ';
        if(dep->operatorType == LITERAL) {
            finalMessage += static_cast<VariableValue*>(dep->value)->getStringValue();
        }
        if(dep->operatorType == VARIABLE) {
            auto* v = static_cast<Variable*>(dep->value);
            finalMessage += v->getValue()->getStringValue();
        }
    }
    return finalMessage;
}
