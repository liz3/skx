//
// Created by liz3 on 16/08/2020.
//

#ifndef SKX_MAPEFFECTS_H
#define SKX_MAPEFFECTS_H

#include "../Instruction.h"

namespace skx {
    class MapEffects : public Execution {
    public:
        enum MapEffectType {
            CONTAINS
        };
        MapEffectType execType;
        OperatorPart * execute(Context *target) override;
    };
}


#endif //SKX_MAPEFFECTS_H
