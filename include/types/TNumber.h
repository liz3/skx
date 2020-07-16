//
// Created by liz3 on 10/07/2020.
//

#ifndef SKX_TNUMBER_H
#define SKX_TNUMBER_H

#include "../Variable.h"

namespace skx {
    class TNumber : public VariableValue {
    public:
        int32_t intValue = 0;
        double doubleValue = 0.00;
        bool isDouble = false;

        TNumber();
        TNumber(int32_t intValue);
        TNumber(double doubleValue);
        bool isEqual(VariableValue *other) override;
        bool isSmaller(VariableValue *other) override;
        bool isBigger(VariableValue *other) override;
        bool isBiggerOrEquals(VariableValue *other) override;
        bool isSmallerOrEquals(VariableValue *other) override;

        bool assign(VariableValue *source) override;
        bool add(VariableValue *source) override;
        bool subtract(VariableValue *source) override;
        bool divide(VariableValue *source) override;
        bool multiply(VariableValue *source) override;
        std::string getStringValue() override;
        VariableValue * copyValue() override;
    };
}


#endif //SKX_TNUMBER_H
