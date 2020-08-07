//
// Created by Liz3 on 03/08/2020.
//

#include "../../include/api/EventPreconditions.h"
#include "../../include/utils.h"
#include "../../include/types/TString.h"
#include "../../include/api/McEventValues.h"

void skx::EventPreconditions::compilePreConditions(std::string content, skx::TriggerEvent *target, Context* ctx) {
    if(target->eventClass == "org.bukkit.event.player.PlayerInteractEvent") {
        //Using item
        if(content.find("with") != std::string::npos) {
            auto params = skx::Utils::split(content.substr(content.find("with") + 5), " ");
            for(auto& param : params){
                if(param == "or") continue;
                Comparison* comparison = new Comparison();
                comparison->type = EQUAL;
                auto strVal =
                        param[param.length() -1] == ':' ? param.substr(0, param.length() -1) : param;
                comparison->source = new OperatorPart(LITERAL, STRING, new TString(strVal), false);
                InteractItem* item = new InteractItem();
                item->ref = target;
                comparison->target = new OperatorPart(EXECUTION, STRING, item, false);
                target->preDefinedConditions.push_back(comparison);

            }
            //targeted item
        } else if (content.substr(2).find("on") != std::string::npos) {
            std::string truncated = content.substr(2);
            auto params = skx::Utils::split(truncated.substr(truncated.find("on") + 3), " ");
            for(auto& param : params){
                if(param == "or") continue;
                Comparison* comparison = new Comparison();
                comparison->type = EQUAL;
                auto strVal =
                        param[param.length() -1] == ':' ? param.substr(0, param.length() -1) : param;
                comparison->source = new OperatorPart(LITERAL, STRING, new TString(strVal), false);
                TargetedItem* item = new TargetedItem();
                item->ref = target;
                comparison->target = new OperatorPart(EXECUTION, STRING, item, false);
                target->preDefinedConditions.push_back(comparison);

            }
        }
    }
}
