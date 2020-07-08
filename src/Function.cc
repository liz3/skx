//
// Created by liz3 on 29/06/2020.
//

#include "../include/Function.h"

skx::OperatorPart *skx::Function::run(std::vector<OperatorPart *> execVars, Context *callingContext) {
    if (execVars.size() != targetParams.size()) return nullptr; //param len mismatch
    OperatorPart *returnValue = nullptr;
    //setup stack
    for (int i = 0; i < targetParams.size(); ++i) {
        auto currentTarget = targetParams[i];
        auto targetVar = execVars[i];
        if (targetVar->operatorType == VARIABLE) {
            Variable *tVar = static_cast<Variable *>(targetVar->value);
            Variable *funcVar = functionItem->ctx->vars[currentTarget->name];
            skx::Utils::copyVariableValue(tVar, funcVar);
        } else if (targetVar->operatorType == DESCRIPTOR) {
            Variable *funcVar = functionItem->ctx->vars[currentTarget->name];
            Variable *tVar = nullptr;
            VariableDescriptor *descriptor = static_cast<VariableDescriptor *>(targetVar->value);
            if (descriptor->type == STATIC || descriptor->type == GLOBAL) {
                tVar = skx::Utils::searchRecursive(descriptor->name, callingContext->global);
            } else {
                tVar = skx::Utils::searchRecursive(descriptor->name, callingContext);
            }
            if (!tVar) return nullptr;
            skx::Utils::copyVariableValue(tVar, funcVar);
        } else if (targetVar->operatorType == LITERAL) {
            Variable *funcVar = functionItem->ctx->vars[currentTarget->name];
            funcVar->type = targetVar->type;
            funcVar->value = targetVar->value;
        }
    }
    ReturnOpWithCtx *returnVal;
    CompileItem *item = functionItem;
    skx::Utils::updateVarState(item->ctx, RUNTIME_CR);
    for (auto current : item->children) {
        returnVal = walk(current);
        if (returnVal != nullptr) break;
    }
    skx::Utils::updateVarState(item->ctx, SPOILED);
    if (returnVal != nullptr) {
        if (returnVal->descriptor->targetReturnItem->operatorType == DESCRIPTOR) {
            Variable *var = nullptr;
            auto *descriptor = static_cast<VariableDescriptor *>(returnVal->descriptor->targetReturnItem->value);
            if (descriptor->type == STATIC || descriptor->type == GLOBAL) {
                var = skx::Utils::searchRecursive(descriptor->name, callingContext->global);
            } else {
                var = skx::Utils::searchRecursive(descriptor->name, returnVal->ctx);
            }
            if (!var) return nullptr;
            return new OperatorPart(VARIABLE, var->type, var, var->isDouble);
        }
        returnValue = returnVal->descriptor->targetReturnItem;

    }

    return returnValue;
}

skx::ReturnOpWithCtx *skx::Function::walk(skx::CompileItem *item) {
    if (item->returner != nullptr) {
        auto *r = new ReturnOpWithCtx();
        r->descriptor = item->returner;
        r->ctx = item->ctx;
        return r;
    }
    if (item->triggers.empty() && item->assignments.size() == 0 && item->comparisons.size() > 0) {
        for (int i = 0; i < item->comparisons.size(); ++i) {
            auto current = item->comparisons[i];
            bool failed = !current->execute(item->ctx);
            if (failed) return nullptr;
        }

        for (auto child : item->children) {
            skx::Utils::updateVarState(child->ctx, RUNTIME_CR);
            auto out = walk(child);
            skx::Utils::updateVarState(child->ctx, SPOILED);
            if (out != nullptr) return out;
        }
        return nullptr;
    }
    if (item->triggers.empty() && item->assignments.size() > 0 && item->comparisons.size() == 0) {
        for (int i = 0; i < item->assignments.size(); ++i) {
            auto current = item->assignments[i];
            bool failed = !current->execute(item->ctx);
            if (failed) return nullptr;
        }
        //assignments should not have children
    }
    if (!item->executions.empty()) {
        for (int i = 0; i < item->executions.size(); ++i) {
            auto current = item->executions[i];
            current->execute(item->ctx);
        }
    }
    return nullptr;
}
