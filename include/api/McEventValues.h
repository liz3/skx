//
// Created by liz3 on 16/07/2020.
//

#ifndef SKX_MCEVENTVALUES_H
#define SKX_MCEVENTVALUES_H

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
            name = "mc::event::interactItem";
        }

        virtual ~TargetedItem();

        TriggerEvent* ref;
        OperatorPart * execute(Context *target) override;
    };
}


#endif //SKX_MCEVENTVALUES_H
