//
// Created by liz3 on 16/07/2020.
//

#include "../../include/api/PlayerEffects.h"
#include "../../include/utils.h"
#include "../../include/Literal.h"
#include "../../include/types/TString.h"
#include "../../include/JnuUtils.h"

skx::OperatorPart *skx::PlayerAction::execute(skx::Context *target) {
    if(targetType == SEND_MESSAGE) {
        jobject player = nullptr;
        JNIEnv* env = nullptr;
        {
            if(ref == nullptr || ref->env == nullptr) return nullptr;
            if(ref != nullptr) {
                env = ref->env;
                player = skx::JniUtils::getPlayerFromEvent(env, ref->currEventRef);
            }
        }
        if(player != nullptr) {
            std::string finalMessage = skx::Utils::depListToString(dependencies);
            jmethodID id = env->GetMethodID(env->FindClass("org/bukkit/entity/Player"), "sendMessage", "(Ljava/lang/String;)V");
            env->CallObjectMethod(player, id, env->NewStringUTF(finalMessage.c_str()));
            if(!id) return nullptr;

        }
    }
    if(targetType == KICK) {
        jobject player = nullptr;
        JNIEnv* env = nullptr;
        {
            if(ref == nullptr || ref->env == nullptr) return nullptr;
            if(ref != nullptr) {
                env = ref->env;
                player = skx::JniUtils::getPlayerFromEvent(env, ref->currEventRef);

            }
        }
        if(player != nullptr) {
            std::string finalMessage = skx::Utils::depListToString(dependencies);

            jmethodID id = env->GetMethodID(env->FindClass("org/bukkit/entity/Player"), "kickPlayer", "(Ljava/lang/String;)V");
            env->CallObjectMethod(player, id, env->NewStringUTF(finalMessage.c_str()));
            if(!id) return nullptr;

        }
    }
    return nullptr;
}

skx::Execution *
skx::PlayerAction::compilePlayerInstruction(std::string &content, skx::Context *pContext, CompileItem *pItem) {
    PlayerAction *action = new PlayerAction();
    auto* root = pItem->root->triggers[0];
    if(root->type == MC_EVENT) {
        action->ref = dynamic_cast<TriggerEvent*>(root);
    } else {
        delete action;
        return nullptr;
    }
    if (content.find("send") == 0 && content.find("to player") != std::string::npos) {
        action->targetType = SEND_MESSAGE;
        uint32_t pos = 0;
        auto split = skx::Utils::split(content, " ");
        for (auto &current : split) {
            if (TreeCompiler::isVar(current)) {
                auto *desc = skx::Variable::extractNameSafe(current);
                Variable *propVar = skx::Utils::searchVar(desc, pContext);
                delete desc;
                if (!propVar) return nullptr;
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
        return action;
    }
    if (content.find("kick") == 0 && content.find("player") != std::string::npos) {
        action->targetType = KICK;
        uint32_t pos = 0;
        auto split = skx::Utils::split(content, " ");
        for (auto &current : split) {
            if (TreeCompiler::isVar(current)) {
                auto *desc = skx::Variable::extractNameSafe(current);
                Variable *propVar = skx::Utils::searchVar(desc, pContext);
                delete desc;
                if (!propVar) return nullptr;
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
        return action;
    }
    delete action;
    return nullptr;
}

skx::PlayerAction::~PlayerAction() {

}
