//
// Created by liz3 on 16/08/2020.
//

#include "../../include/types/TPlayerRef.h"

skx::TPlayerRef::TPlayerRef() {

}

std::string skx::TPlayerRef::getStringValue() {
    if(varRef == nullptr) return "";
    if(eventRef != nullptr /*|| command != nullptr*/) {
        return *static_cast<std::string*>((playerName->execute(varRef->ctx)->value));
    }
    return "";
}

skx::TPlayerRef::TPlayerRef(skx::TriggerEvent *eventRef) : eventRef(eventRef) {
    playerName = new PlayerName();
    playerName->ref = eventRef;
}

skx::TPlayerRef::TPlayerRef(skx::TriggerCommand *command) : command(command) {}
