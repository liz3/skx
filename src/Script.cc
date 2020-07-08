//
// Created by liz3 on 29/06/2020.
//

#include "../include/Script.h"
#include <string>
#include "../include/utils.h"
#include "../include/Context.h"
#include "../include/BaseTypeParser.h"

#include <algorithm>
#include <iostream>

/**
 * Walk over script tree like when pre parsing and passing down subsequential contexts
 */


skx::Script *skx::Script::parse(char *input) {
    std::string base(input);
    base.erase(std::remove(base.begin(), base.end(), '\r'), base.end());
    auto lines = skx::Utils::split(base, "\n");
    auto parseResult = skx::PreParser::preParse(lines);
    Script* result = new Script(new Context(0, parseResult->rootItems.size(), 0, nullptr));
    result->baseContext->global = result->baseContext;
    for (auto & walkItem : parseResult->rootItems) {
        auto baseType = skx::BaseAction::getBaseType(walkItem->actualContent);
        if(baseType == OPTIONS) {
            for(auto& opt : walkItem->children) {
                Variable::createVarFromOption(opt->actualContent, result->baseContext, true);
            }
        }
    }
    for (auto & walkItem : parseResult->rootItems) {
        auto baseType = skx::BaseAction::getBaseType(walkItem->actualContent);
        if(baseType == FUNCTION) {
            skx::TreeCompiler::compileTreeFunction(walkItem, result->baseContext);
        }
    }
    for (auto & walkItem : parseResult->rootItems) {
        result->walk(walkItem, result->baseContext, result);
        result->baseContext->stepPointer++;
    }
    result->preParseResult = parseResult;
    return result;
}

skx::Script::Script(Context *baseContext) : baseContext(baseContext) {}

void skx::Script::walk(PreParserItem *item, Context *itemContext, Script *script) {
    if(item->level == 0) {
        auto baseType = skx::BaseAction::getBaseType(item->actualContent);
        switch (baseType) {
            case PRE_RUNTIME_EVENT: {
                script->compiledPreRuntimeEvents.push_back(skx::TreeCompiler::compileTree(item, itemContext));
            }

            default:
                break;
        }
    }

}

skx::Script::~Script() {
    for (auto & compiledPreRuntimeEvent : compiledPreRuntimeEvents) {
        delete compiledPreRuntimeEvent;
    }
    delete baseContext;
}
