//
// Created by liz3 on 13/07/2020.
//

#include <iostream>
#include "../../include/api/McEventsBaseEffects.h"

skx::OperatorPart *skx::CancelEvent::execute(skx::Context *target) {
    if(ref == nullptr || ref->env == nullptr) return nullptr;
    jclass cl = ref->env->FindClass("org/bukkit/event/Cancellable");
    jmethodID methodId = ref->env->GetMethodID(cl, "setCancelled", "(Z)V");
    ref->env->CallBooleanMethod(ref->currEventRef, methodId, true);
    return nullptr;
}

skx::CancelEvent::~CancelEvent() {

}


skx::OperatorPart *skx::UpdateChatEventMessage::execute(skx::Context *target) {
    if(ref == nullptr) return nullptr;
    JNIEnv* env = ref->env;
    jmethodID methodId = env->GetMethodID(env->FindClass("org/bukkit/event/player/AsyncPlayerChatEvent"), "setMessage", "(Ljava/lang/String;)V");
    std::string final = "";
    for(auto dep : dependencies) {
        if(dep->operatorType == LITERAL) {
            final += static_cast<VariableValue*>(dep->value)->getStringValue();
        } else if (dep->operatorType == VARIABLE) {
            Variable* v = static_cast<Variable*>(dep->value);
            final += v->getValue()->getStringValue();
        }
    }
    std::cout << final << "\n";
    env->CallObjectMethod(ref->currEventRef, methodId, env->NewStringUTF(final.c_str()));
    return nullptr;
}

skx::UpdateChatEventMessage::~UpdateChatEventMessage() {

}
