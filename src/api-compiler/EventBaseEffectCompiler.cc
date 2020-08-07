//
// Created by Yann Holme Nielsen on 07/08/2020.
//

#include "../../include/api-compiler/EventBaseEffectCompiler.h"
#include "../../include/api/McEventsBaseEffects.h"
#include "../../include/utils.h"
#include "../../include/Literal.h"
#include "../../include/types/TString.h"

void
skx::EventBaseEffectCompiler::compile(std::string &content, skx::Context *ctx, skx::CompileItem *target) {
    CompileItem *t = target->root;
    if (content == "cancel event" || content == "cancel the event") {
        CancelEvent *cancel = new CancelEvent();
        if (t->triggers.size() == 1) {
            cancel->ref = dynamic_cast<TriggerEvent *>(t->triggers[0]);
            target->executions.push_back(cancel);
            return;
        }
        delete cancel;
        return;
    }
    if(content.find("update chat message to") == 0 &&
    dynamic_cast<TriggerEvent *>(t->triggers[0])->eventClass == "org.bukkit.event.player.AsyncPlayerChatEvent") {
        uint32_t pos = 23;

        auto* action = new UpdateChatEventMessage();
        action->ref = dynamic_cast<TriggerEvent *>(t->triggers[0]);
        auto params = skx::Utils::split(content.substr(23), " ");
        for (auto &current : params) {
            if (TreeCompiler::isVar(current)) {
                auto *desc = skx::Variable::extractNameSafe(current);
                Variable *propVar = skx::Utils::searchVar(desc, ctx);
                delete desc;
                if (!propVar)
                    return;
                action->dependencies.push_back(new OperatorPart(VARIABLE, propVar->type, propVar, propVar->isDouble));
            } else if (TreeCompiler::isNumber(current[0])) {
                action->dependencies.push_back(skx::Literal::extractNumber(current));
            } else if(current[0] == '"') {
                std::string stringVal = "";
                std::string start = content.substr(pos + 1);
                for (int i = 0; i < start.length(); ++i) {
                    if (start[i] == '"') break;
                    if (start[i] == '\\' && i < start.length() - 1 && start[i + 1] == '"') {
                        stringVal.append("\"");
                        i += 2;
                        continue;
                    }
                    pos++;
                    stringVal += start[i];
                }
                action->dependencies.push_back(new OperatorPart(LITERAL, STRING, new TString(stringVal), false));
            }
            pos += current.length() + 1;
        }
        target->executions.push_back(action);
    }
}
