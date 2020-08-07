//
// Created by liz3 on 16/07/2020.
//

#ifndef SKX_RUNTIMEMCEVENTVALUES_H
#define SKX_RUNTIMEMCEVENTVALUES_H

#include "../Instruction.h"

namespace skx {
    class PlayerName : public Execution {
    public:
        PlayerName() : Execution() {
            name = "mc::event::playerName";
        }

        virtual ~PlayerName();

        TriggerEvent* ref;
        OperatorPart * execute(Context *target) override;
    };
    class ChatMessage : public Execution {
    public:
        ChatMessage() : Execution() {
            name = "mc::event::playerName";
        }

        virtual ~ChatMessage();

        TriggerEvent* ref;
        OperatorPart * execute(Context *target) override;
    };
    class InteractItem : public Execution {
    public:
        InteractItem() : Execution() {
            name = "mc::event::interactItem";
        }

        virtual ~InteractItem();

        TriggerEvent* ref;
        OperatorPart * execute(Context *target) override;
    };
    class TargetedItem : public Execution {
    public:
        TargetedItem() : Execution() {
            name = "mc::event::targetedItem";
        }

        virtual ~TargetedItem();

        TriggerEvent* ref;
        OperatorPart * execute(Context *target) override;
    };
}


#endif //SKX_RUNTIMEMCEVENTVALUES_H
