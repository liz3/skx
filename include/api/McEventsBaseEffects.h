//
// Created by liz3 on 13/07/2020.
//

#ifndef SKX_MCEVENTSBASEEFFECTS_H
#define SKX_MCEVENTSBASEEFFECTS_H

#include "../Instruction.h"

namespace skx {
    class CancelEvent : public Execution {
    public:
        CancelEvent() : Execution() {
            name = "mc::event::cancel";
        }

        virtual ~CancelEvent();

        TriggerEvent* ref;
        OperatorPart * execute(Context *target) override;
    };
    class UpdateChatEventMessage : public Execution {
    public:
        UpdateChatEventMessage() : Execution() {
            name = "mc::event::UpdateChatEventMessage";
        }

        virtual ~UpdateChatEventMessage();

        TriggerEvent* ref;
        OperatorPart * execute(Context *target) override;
    };
}


#endif //SKX_MCEVENTSBASEEFFECTS_H
