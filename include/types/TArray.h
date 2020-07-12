//
// Created by liz3 on 10/07/2020.
//

#ifndef SKX_TARRAY_H
#define SKX_TARRAY_H

#include "../Variable.h"
#include <vector>

namespace skx {
    class TArray : public VariableValue {
    public:
        TArray();

        std::vector<VariableValue*> entries;
        bool assign(VariableValue *source) override;
        bool add(VariableValue *source) override;

        bool isEqual(VariableValue *other) override;
        bool isSmaller(VariableValue *other) override;
        bool isBigger(VariableValue *other) override;
        bool isSmallerOrEquals(VariableValue *other) override;
        bool isBiggerOrEquals(VariableValue *other) override;

        VariableValue * copyValue() override;
    };
}


#endif //SKX_TARRAY_H
