//
// Created by liz3 on 30/06/2020.
//

#include "../include/TreeCompiler.h"
#include "../include/utils.h"
#include "../include/stdlib.h"
#include "../include/Literal.h"
#include "../include/RegexUtils.h"

#include "../include/Function.h"

#include <exception>

skx::CompileItem *skx::TreeCompiler::compileTree(skx::PreParserItem *item, skx::Context *ctx) {
    TreeCompiler compiler;
    CompileItem *root = new CompileItem();
    root->ctx = ctx;
    root->level = 0;
    compiler.compileExpression(item, ctx, root);
    compiler.advance(root, item);
    return root;
}

skx::CompileItem *skx::TreeCompiler::compileTreeFunction(skx::PreParserItem *item, skx::Context *ctx) {
    TreeCompiler compiler;
    CompileItem *root = new CompileItem();
    Function *func = new Function();
    compiler.setupFunctionMeta(item->actualContent, func);
    Context *functionCtx = new Context(0, item->children.size(), 0, ctx);
    functionCtx->global = ctx->global;
    for (int i = 0; i < func->targetParams.size(); ++i) {
        auto current = func->targetParams[i];
        Variable *param = new Variable();
        param->name = current->name;
        param->accessType = current->type;
        param->value = nullptr;
        param->type = UNDEFINED;
        functionCtx->vars[param->name] = param;
    }
    root->ctx = functionCtx;
    compiler.advance(root, item, true);
    func->functionItem = root;
    ctx->functions[func->name] = func;
    return root;
}

void skx::TreeCompiler::compileExpression(skx::PreParserItem *item, Context *context, CompileItem *target) {

    if (item->level == 0) {
        Trigger *trigger = new Trigger();
        trigger->type = item->actualContent == "on load:" ? BOOT : EVENT;
        target->triggers.push_back(trigger);
    } else {
        auto actualContent = item->actualContent;
        if (actualContent == "else:") {
            target->isElse = true;
            return;
        }
        if (actualContent.rfind("if", 0) == 0) {
            compileCondition(actualContent, context, target);
        } else if (actualContent.rfind("else if", 0) == 0) {
            compileCondition(actualContent, context, target);
        } else if (actualContent.rfind("set", 0) == 0) {
            compileAssigment(actualContent, context, target);
        }  else if (actualContent.rfind("return", 0) == 0) {
            compileReturn(actualContent, context, target);
        } else {
            compileExecution(actualContent, context, target);
        }
    }

}

void skx::TreeCompiler::advance(skx::CompileItem *parent, skx::PreParserItem *parentItem, bool isFuncSuperContext) {
    if (parentItem->children.size() == 0) return;
    Context *thisCtx = isFuncSuperContext ? parent->ctx : new Context(parent->level + 1, parentItem->children.size(), 0,
                                                                      parent->ctx);
    thisCtx->global = parent->ctx->global;
    for (PreParserItem *item : parentItem->children) {
        CompileItem *next = new CompileItem();
        next->ctx = thisCtx;
        next->level = parent->level + 1;
        compileExpression(item, thisCtx, next);
        advance(next, item);
        parent->children.push_back(next);
    }
}

void
skx::TreeCompiler::compileCondition(std::string &content, skx::Context *ctx, skx::CompileItem *target, bool isElseIf) {
    auto spaceSplit = skx::Utils::split(content.substr(isElseIf ? 7 : 3), " ");
    target->isElseIf = isElseIf;
    Comparison *currentOperator = nullptr;
    uint8_t state = 0;
    std::string last = "";
    for (int i = 0; i < spaceSplit.size(); ++i) {
        auto current = spaceSplit[i];
        if(current[current.length() -1] == ':') {
            current = current.substr(0, current.length() - 1);
        }
        if (current.rfind('"', 0) == 0 && state == 2) {
            int x = i;
            while (spaceSplit[i][spaceSplit[i].length() - 1] != '"' && i < spaceSplit.size() - 1) {
                i++;
                if (i != x)
                    current = current.append(" " + spaceSplit[i]);

            }
            if (current[current.length() - 1] == ':') current = current.substr(0, current.length() - 1);
            auto *f = new std::string(current.substr(1, current.length() - 2));
            currentOperator->target = new OperatorPart(LITERAL, STRING, f, false);
            target->comparisons.push_back(currentOperator);
            state = 0;
            currentOperator = nullptr;
        }
        if ((current == "true" || current == "false") && state == 2) {
            currentOperator->target = new OperatorPart(LITERAL, BOOLEAN, new bool(current == "true"), false);

            target->comparisons.push_back(currentOperator);
            state = 0;
            currentOperator = nullptr;
        }
        if (current[0] == '{' && current[current.length() - 1] == '}') {
            if (state == 0) {
                state++;
                currentOperator = new Comparison();
                auto descriptor = skx::Variable::extractNameSafe(current);
                Variable *sourceVar = nullptr;
                if (descriptor->type == STATIC || descriptor->type == GLOBAL) {
                    sourceVar = skx::Utils::searchRecursive(descriptor->name, ctx->global);
                } else {
                    sourceVar = skx::Utils::searchRecursive(descriptor->name, ctx);
                }
                currentOperator->source = new OperatorPart(VARIABLE, sourceVar->type, sourceVar, sourceVar->isDouble);
            } else if (state == 2) {
                auto descriptorTwo = skx::Variable::extractNameSafe(current);
                Variable *targetVar = nullptr;
                if (descriptorTwo->type == STATIC || descriptorTwo->type == GLOBAL) {
                    targetVar = skx::Utils::searchRecursive(descriptorTwo->name, ctx->global);
                } else {
                    targetVar = skx::Utils::searchRecursive(descriptorTwo->name, ctx);
                }
                currentOperator->target = new OperatorPart(VARIABLE, targetVar->type, targetVar, targetVar->isDouble);
                target->comparisons.push_back(currentOperator);
                state = 0;
                currentOperator = nullptr;


            }
        }
        if (currentOperator != nullptr) {
            if (current == "is") {
                currentOperator->type = EQUAL;
                if (state < 2) state++;
            }
            if (current == "equal") {
                if ((currentOperator->type == SMALLER || currentOperator->type == BIGGER) && last == "or") {
                    if (currentOperator->type == SMALLER) currentOperator->type = SMALLER_OR_EQUAL;
                    if (currentOperator->type == BIGGER) currentOperator->type = BIGGER_OR_EQUAL;
                    if (state < 2) state++;

                }
            }
            if (current == "greater") {
                currentOperator->type = BIGGER;
                if (state < 2) state++;
            }
            if (current == "smaller") {
                currentOperator->type = SMALLER;
                if (state < 2) state++;
            }
            if (current == "not") {
                if (currentOperator->type == EQUAL) {
                    currentOperator->type = NOT_EQUAL;
                    if (state < 2) state++;
                }
            }
        }
        last = current;
    }
}

void skx::TreeCompiler::compileAssigment(std::string content, skx::Context *ctx, skx::CompileItem *target) {
    auto spaceSplit = skx::Utils::split(content.substr(4), " ");
    Assigment *assigment = nullptr;
    uint8_t step = 0;
    bool created = false;
    for (int i = 0; i < spaceSplit.size(); ++i) {
        auto current = spaceSplit[i];
        if (current.rfind('"', 0) == 0 && step == 2) {
            int x = i;
            while (spaceSplit[i][spaceSplit[i].length() - 1] != '"' && i < spaceSplit.size() - 1) {
                i++;
                if (i != x)
                    current = current.append(" " + spaceSplit[i]);

            }
            if (current[current.length() - 1] == ':') current = current.substr(0, current.length() - 1);
            auto *f = new std::string(current.substr(1, current.length() - 2));
            assigment->source = new OperatorPart(LITERAL, STRING, f, false);
            target->assignments.push_back(assigment);
            step = 0;
            assigment = nullptr;
        }
        if(step == 2) {
            auto funcCallMatches = skx::RegexUtils::getMatches(skx::functionCallPattern, content);
            if (!(funcCallMatches).empty()) {
                auto entry = funcCallMatches[0];
                std::string base = entry.content;
                std::string name = base.substr(0, base.find_first_of(" \n\r\t\f\v("));
                size_t paramsStart = base.find_first_of('(');
                size_t paramsEnd = base.find_last_of(')');
                std::string params = base.substr(paramsStart + 1, paramsEnd - paramsStart - 1);
                auto *call = new FunctionInvoker();
                call->function = ctx->global->functions[name];

                for (auto const &param : skx::Utils::split(params, ",")) {
                    auto trimmed = skx::Utils::trim(param);
                    auto descriptor = skx::Variable::extractNameSafe(trimmed);

                    call->dependencies.push_back(new OperatorPart(DESCRIPTOR, UNDEFINED, descriptor, false));
                }
                assigment->source = new OperatorPart(EXECUTION, UNDEFINED, call, false);
                target->assignments.push_back(assigment);
                assigment = nullptr;
                step = 0;
                created = false;
            }

        }

        if (current[0] == '{' && current[current.length() - 1] == '}') {
            auto descriptor = skx::Variable::extractNameSafe(current);
            Variable *currentVar = nullptr;
            if (descriptor->type == STATIC || descriptor->type == GLOBAL) {
                currentVar = skx::Utils::searchRecursive(descriptor->name, ctx->global);
            } else {
                currentVar = skx::Utils::searchRecursive(descriptor->name, ctx);
            }
            if (currentVar == nullptr) {
                created = true;
                currentVar = new Variable();
                currentVar->name = descriptor->name;
                currentVar->accessType = descriptor->type;
                currentVar->value = nullptr;
                currentVar->ctx = ctx;
                ctx->vars[descriptor->name] = currentVar;
            }
            if (assigment == nullptr || assigment->target == nullptr) {
                assigment = new Assigment();
                assigment->target = new OperatorPart(VARIABLE, currentVar->type, currentVar, currentVar->isDouble);
                step++;
            } else {
                if (step == 2) {
                    assigment->source = new OperatorPart(VARIABLE, currentVar->type, currentVar, currentVar->isDouble);
                    if (created) {
                        static_cast<Variable *>(assigment->target->value)->type = currentVar->type;
                        assigment->target->type = currentVar->type;

                    }
                    target->assignments.push_back(assigment);
                    assigment = nullptr;
                    step = 0;
                    created = false;
                }
            }
        }

        if (assigment != nullptr) {
            if (current == "to") {
                assigment->type = ASSIGN;
                if (step < 2) step++;
            }
        }
    }
}

void skx::TreeCompiler::setupFunctionMeta(std::string &content, skx::Function *target) {
    auto base = content.substr(8);
    std::string name = skx::Utils::ltrim(base);
    name = name.substr(0, name.find_first_of(" \n\r\t\f\v("));
    size_t paramsStart = base.find_first_of('(');
    size_t paramsEnd = base.find_last_of(')');
    std::string params = base.substr(paramsStart + 1, paramsEnd - paramsStart - 1);
    for (auto const &param : skx::Utils::split(params, ",")) {
        auto trimmed = skx::Utils::trim(param);
        ValueDescriptor *descriptor = new ValueDescriptor();
        if (trimmed.find(':') == std::string::npos) {
            descriptor->type = CONTEXT;
            descriptor->name = trimmed;
        } else {
            auto typeSplit = skx::Utils::split(trimmed, ":");
            auto actualName = skx::Utils::trim(typeSplit[0]);
            //TODO implement actual param type & default value, also what if the default value has a ,?
            //auto actualType = skx::Utils::trim(typeSplit[1]);
            descriptor->type = CONTEXT;
            descriptor->name = actualName;
        }
        target->targetParams.push_back(descriptor);
    }
    target->name = name;
    int x = 22;
}

void skx::TreeCompiler::compileExecution(std::string &content, skx::Context *context, skx::CompileItem *target) {
    if (content.find("to console") != std::string::npos) {
        auto *pr = new Print();
        auto spaceSplit = skx::Utils::split(content, " ");
        size_t pos = 0;
        for (int i = 0; i < spaceSplit.size(); i++) {
            auto current = spaceSplit[i];
            if (current.rfind('"', 0) == 0) {
                int x = i;
                while (spaceSplit[i][spaceSplit[i].length() - 1] != '"' && i < spaceSplit.size() - 1) {
                    i++;
                    if (i != x)
                        current = current.append(" " + spaceSplit[i]);

                }
                if (current[current.length() - 1] == ':') current = current.substr(0, current.length() - 1);
                auto *f = new std::string(current.substr(1, current.length() - 2));
                pr->dependencies.push_back(new OperatorPart(LITERAL, STRING, f, false));

            }

            if (current[0] == '{' && current[current.length() - 1] == '}') {
                auto descriptor = skx::Variable::extractNameSafe(current);
                pr->dependencies.push_back(new OperatorPart(DESCRIPTOR, UNDEFINED, descriptor, false));
            }
            pos += current.length();
        }
        target->executions.push_back(pr);
        return;
    }
    auto funcCallMatches = skx::RegexUtils::getMatches(skx::functionCallPattern, content);
    if (!(funcCallMatches).empty()) {
        auto entry = funcCallMatches[0];
        std::string base = entry.content;
        std::string name = base.substr(0, base.find_first_of(" \n\r\t\f\v("));
        size_t paramsStart = base.find_first_of('(');
        size_t paramsEnd = base.find_last_of(')');
        std::string params = base.substr(paramsStart + 1, paramsEnd - paramsStart - 1);
        auto *call = new FunctionInvoker();
        call->function = context->global->functions[name];

        for (auto const &param : skx::Utils::split(params, ",")) {
            auto trimmed = skx::Utils::trim(param);
            auto descriptor = skx::Variable::extractNameSafe(trimmed);

            call->dependencies.push_back(new OperatorPart(DESCRIPTOR, UNDEFINED, descriptor, false));
        }
        target->executions.push_back(call);
    }

}

void skx::TreeCompiler::compileReturn(std::string &basicString, skx::Context *pContext, skx::CompileItem *pItem) {
    auto spaceSplit = skx::Utils::split(basicString.substr(7), " ");

    for (int i = 0; i < spaceSplit.size(); i++) {
        auto current = spaceSplit[i];
        if (current.rfind('"', 0) == 0) {
            int x = i;
            while (spaceSplit[i][spaceSplit[i].length() - 1] != '"' && i < spaceSplit.size() - 1) {
                i++;
                if (i != x)
                    current = current.append(" " + spaceSplit[i]);

            }
            if (current[current.length() - 1] == ':') current = current.substr(0, current.length() - 1);
            auto *f = new std::string(current.substr(1, current.length() - 2));
            pItem->returner = new ReturnOperation();
            pItem->returner->targetReturnItem = new OperatorPart(LITERAL, STRING, f, false);

        }

        if (current[0] == '{' && current[current.length() - 1] == '}') {
            auto descriptor = skx::Variable::extractNameSafe(current);
            pItem->returner = new ReturnOperation();
            pItem->returner->targetReturnItem= new OperatorPart(DESCRIPTOR, UNDEFINED, descriptor, false);
        }

    }
}
