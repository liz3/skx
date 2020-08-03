//
// Created by Liz3 on 03/08/2020.
//

#include "../../include/api/EventPreconditions.h"
#include "../../include/utils.h"
#include "../../include/types/TString.h"
#include "../../include/api/McEventValues.h"

void skx::EventPreconditions::compilePreConditions(std::string content, skx::TriggerEvent *target, Context* ctx) {
    if(target->eventClass == "org.bukkit.event.player.PlayerInteractEvent") {
        if(content.find("with") != std::string::npos) {
            auto params = skx::Utils::split(content.substr(content.find("with") + 1), " ");
            for(auto& param : params){
                if(param == "or") continue;
                Comparison* comparison = new Comparison();
                comparison->type = EQUAL;
                comparison->source = new OperatorPart(LITERAL, STRING, new TString(param), false);
                InteractItem* item = new InteractItem();
                item->ref = target;
                comparison->target = new OperatorPart(EXECUTION, STRING, item, false);
                target->preDefinedConditions.push_back(comparison);
            }
        }
    }
}
