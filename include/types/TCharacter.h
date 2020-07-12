//
// Created by liz3 on 10/07/2020.
//

#ifndef SKX_TCHARACTER_H
#define SKX_TCHARACTER_H

#include "../Variable.h"

namespace skx {
    class TCharacter : public VariableValue {
    public:
        char value;

        TCharacter();

        TCharacter(char value);

        bool assign(VariableValue *source) override;
        bool isEqual(VariableValue *other) override;
        bool isBigger(VariableValue *other) override;
        bool isSmaller(VariableValue *other) override;
        bool isBiggerOrEquals(VariableValue *other) override;
        bool isSmallerOrEquals(VariableValue *other) override;

        VariableValue * copyValue() override;
    };
}


#endif //SKX_TCHARACTER_H
