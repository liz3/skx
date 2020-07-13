//
// Created by liz3 on 13/07/2020.
//

#ifndef SKX_MCEVENTS_H
#define SKX_MCEVENTS_H

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
}


#endif //SKX_MCEVENTS_H
