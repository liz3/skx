//
// Created by liz3 on 09/07/2020.
//

#include "../../include/types/TString.h"

skx::TString::TString(std::string value) : value(value) {
    type = STRING;
}

skx::TString::TString(std::string value, bool readOnly) {
    isReadOnly = readOnly;
}

skx::TString::TString() {
    type = STRING;
}

bool skx::TString::isEqual(skx::VariableValue *other) {
    TString* val = dynamic_cast<TString*>(other);
    return value == val->value;
}

bool skx::TString::isSmaller(skx::VariableValue *other) {
    TString* val = dynamic_cast<TString*>(other);
    return value.length() < val->value.length();
}

bool skx::TString::isBigger(skx::VariableValue *other) {
    TString* val = dynamic_cast<TString*>(other);
    return value.length() > (*val).value.length();
}

bool skx::TString::isSmallerOrEquals(skx::VariableValue *other) {
    TString* val = dynamic_cast<TString*>(other);
    return value.length() <= (*val).value.length();
}

bool skx::TString::isBiggerOrEquals(skx::VariableValue *other) {
    TString* val = dynamic_cast<TString*>(other);
    return value.length() >= (*val).value.length();
}

bool skx::TString::assign(skx::VariableValue *source) {
    if(isReadOnly) return false;
    TString* val = dynamic_cast<TString*>(source);
    this->value = val->value;
    return true;
}

bool skx::TString::add(skx::VariableValue *source) {
    if(isReadOnly) return false;
    TString* val = dynamic_cast<TString*>(source);
    this->value = value.append(val->value);
    return true;
}

bool skx::TString::subtract(skx::VariableValue *source) {
    return VariableValue::subtract(source);
}

bool skx::TString::multiply(skx::VariableValue *source) {
    return VariableValue::multiply(source);
}

bool skx::TString::divide(skx::VariableValue *source) {
    return VariableValue::divide(source);
}

skx::VariableValue *skx::TString::copyValue() {
    TString* str = new TString(value);
    return str;
}
