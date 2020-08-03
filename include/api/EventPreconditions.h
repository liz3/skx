//
// Created by Liz3 on 03/08/2020.
//

#ifndef SKX_EVENTPRECONDITIONS_H
#define SKX_EVENTPRECONDITIONS_H

#include <string>
#include "../Instruction.h"
#include "../Context.h"

namespace skx {
    class EventPreconditions {
    public:
        static void compilePreConditions(std::string content, TriggerEvent* target, Context* ctx);
    };
}


#endif //SKX_EVENTPRECONDITIONS_H
