//
// Created by liz3 on 09/07/2020.
//

#ifndef SKX_TBOOLEAN_H
#define SKX_TBOOLEAN_H

#include "../Variable.h"


namespace skx {
    class TBoolean : public VariableValue {
    public:
        bool value = false;
        TBoolean();
        TBoolean(bool value);
        TBoolean(bool value, bool readOnly);
        bool isEqual(VariableValue *other) override;
        bool isSmaller(VariableValue *other) override;
        bool isBigger(VariableValue *other) override;
        bool isSmallerOrEquals(VariableValue *other) override;
        bool isBiggerOrEquals(VariableValue *other) override;

        bool assign(VariableValue *source) override;
        bool add(VariableValue *source) override;
        bool subtract(VariableValue *source) override;
        bool multiply(VariableValue *source) override;
        bool divide(VariableValue *source) override;
        VariableValue * copyValue() override;
        std::string getStringValue() override;
    };
}


#endif //SKX_TBOOLEAN_H
