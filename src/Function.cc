//
// Created by liz3 on 29/06/2020.
//

#include "../include/Function.h"

skx::Variable* skx::Function::run( std::vector<Variable*> execVars) {
    if(execVars.size() != targetParams.size()) return nullptr; //param len mismatch
    Variable* returnValue = nullptr;
    //setup stack
    for (int i = 0; i < targetParams.size(); ++i) {
        auto currentTarget = targetParams[i];
        auto targetVar = execVars[i];
        Variable* funcVar = functionItem->ctx->vars[currentTarget->name];
        skx::Utils::copyVariableValue(targetVar, funcVar);
    }
    ReturnOpWithCtx* returnVal;
    CompileItem* item = functionItem;
    skx::Utils::updateVarState(item->ctx, RUNTIME_CR);
    for (auto current : item->children) {
        returnVal = walk(current);
        if(returnVal != nullptr) break;
    }
    skx::Utils::updateVarState(item->ctx, SPOILED);
    if(returnVal != nullptr) {
        returnValue = skx::Utils::searchRecursive(returnVal->descriptor->targetReturnItem->name, returnVal->ctx);
    }

    return returnValue;
}

skx::ReturnOpWithCtx* skx::Function::walk(skx::CompileItem *item) {
    if(item->returner != nullptr) {
       auto* r = new ReturnOpWithCtx();
       r->descriptor = item->returner;
       r->ctx = item->ctx;
       return r;
    }
    if(item->triggers.empty() && item->assignments.size() == 0 && item->comparisons.size() > 0) {
        for (int i = 0; i < item->comparisons.size(); ++i) {
            auto current = item->comparisons[i];
            bool failed = !current->execute(item->ctx);
            if(failed) return nullptr;
        }

        for (auto child : item->children) {
            skx::Utils::updateVarState(child->ctx, RUNTIME_CR);
            auto out = walk(child);
            skx::Utils::updateVarState(child->ctx, SPOILED);
            if(out != nullptr) return out;
        }
        return nullptr;
    }
    if(item->triggers.empty() && item->assignments.size() > 0 && item->comparisons.size() == 0) {
        for (int i = 0; i < item->assignments.size(); ++i) {
            auto current = item->assignments[i];
            bool failed = !current->execute(item->ctx);
            if(failed) return nullptr;
        }
        //assignments should not have children
    }
    return nullptr;
}
