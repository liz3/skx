//
// Created by Yann Holme Nielsen on 07/08/2020.
//

#include "../../include/api-compiler/EventValuesCompiler.h"
#include "../../include/api/RuntimeMcEventValues.h"

//COMPILE TIME

skx::OperatorPart* skx::EventValuesCompiler::compile(std::string &content, skx::Context *ctx, skx::CompileItem *target, bool isElseIf) {
    CompileItem *t = target->root;
    if(content == "players name" || content == "player name") {
        PlayerName* name = new PlayerName();
        name->ref = dynamic_cast<TriggerEvent*>(t->triggers[0]);
        return new OperatorPart(EXECUTION, UNDEFINED, name, false);
    }
    if(content == "event-message" || content == "event message") {
        if(dynamic_cast<TriggerEvent*>(t->triggers[0])->eventClass == "org.bukkit.event.player.AsyncPlayerChatEvent") {
            ChatMessage* message = new ChatMessage();
            message->ref = dynamic_cast<TriggerEvent*>(t->triggers[0]);
            return new OperatorPart(EXECUTION, UNDEFINED, message, false);
        } else {

        }
    }
    return nullptr;
}
