//
// Created by liz3 on 13/07/2020.
//

#include <iostream>
#include "../../include/api/McEvents.h"

skx::OperatorPart *skx::CancelEvent::execute(skx::Context *target) {
    if(ref == nullptr || ref->env == nullptr) return nullptr;
    jclass cl = ref->env->FindClass("org/bukkit/event/player/PlayerBedEnterEvent");
    jmethodID methodId = ref->env->GetMethodID(cl, "setCancelled", "(Z)V");
    ref->env->CallBooleanMethod(ref->currEventRef, methodId, true);
    std::cout << "executed\n";
    return nullptr;
}

skx::CancelEvent::~CancelEvent() {

}
