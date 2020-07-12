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


skx::Script *skx::Script::parse(const char *input) {
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
        if(baseType == FUNCTION) {
            skx::TreeCompiler::compileTreeFunction(walkItem, result->baseContext);
        }
    }

    for (auto & walkItem : parseResult->rootItems) {
        auto baseType = skx::BaseAction::getBaseType(walkItem->actualContent);
        if(baseType == FUNCTION || baseType == OPTIONS) continue;
        CompileItem* compileItem = skx::TreeCompiler::compileTree(walkItem, result->baseContext);
        if(compileItem->triggers.size() == 0) {
            std::cout << "[Warning] Empty trigger found? " << walkItem->actualContent << " at " << walkItem->pos << "\n";
            continue;
        }
        Trigger* trigger = compileItem->triggers[0];
        switch (trigger->type) {
            case SIGNAL: {
                TriggerSignal* signal = dynamic_cast<TriggerSignal*>(trigger);
                result->signals[signal] = compileItem;
                break;
            }
            case MC_COMMAND: {
                TriggerCommand* command = dynamic_cast<TriggerCommand*>(trigger);
                result->mc_commands[command] = compileItem;
                break;
            }
            case MC_EVENT: {
                TriggerEvent* event = dynamic_cast<TriggerEvent*>(trigger);
                result->mc_events[event] = compileItem;
                break;
            }

            default:
                break;
        }
        result->baseContext->stepPointer++;
    }
    result->preParseResult = parseResult;
    return result;
}

skx::Script::Script(Context *baseContext) : baseContext(baseContext) {}

skx::Script::~Script() {
    for(const auto& entry : signals) {
        delete entry.second;

    }
    for(const auto& entry : mc_events) {
        delete entry.second;

    }
    for(const auto& entry : mc_commands) {
        delete entry.second;

    }
    delete baseContext;
}
