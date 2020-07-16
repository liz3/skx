//
// Created by liz3 on 09/07/2020.
//

#ifndef SKX_TSTRING_H
#define SKX_TSTRING_H

#include "../Variable.h"

namespace skx {
    class TString : public VariableValue {
    public:
        std::string value = "";
        TString();
        TString(std::string value);
        TString(std::string value, bool readOnly);
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


#endif //SKX_TSTRING_H
