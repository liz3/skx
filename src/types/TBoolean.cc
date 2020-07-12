//
// Created by liz3 on 09/07/2020.
//

#include "../../include/types/TBoolean.h"

skx::TBoolean::TBoolean() {
    type = BOOLEAN;
}

skx::TBoolean::TBoolean(bool value) {
    type = BOOLEAN;

    this->value = value;
}

skx::TBoolean::TBoolean(bool value, bool readOnly) {
    type = BOOLEAN;
    this->value = value;
    this->isReadOnly = readOnly;
}

bool skx::TBoolean::isEqual(skx::VariableValue *other) {
    TBoolean* val = dynamic_cast<TBoolean*>(other);
    return val->value == value;
}

bool skx::TBoolean::isSmaller(skx::VariableValue *other) {
    return VariableValue::isSmaller(other);
}

bool skx::TBoolean::isBigger(skx::VariableValue *other) {
    return VariableValue::isBigger(other);
}

bool skx::TBoolean::isSmallerOrEquals(skx::VariableValue *other) {
    return VariableValue::isSmallerOrEquals(other);
}

bool skx::TBoolean::isBiggerOrEquals(skx::VariableValue *other) {
    return VariableValue::isBiggerOrEquals(other);
}

bool skx::TBoolean::assign(skx::VariableValue *source) {
    if(isReadOnly) return false;
    TBoolean* val = dynamic_cast<TBoolean*>(source);
    value = val->value;
    return true;
}

bool skx::TBoolean::add(skx::VariableValue *source) {
    return VariableValue::add(source);
}

bool skx::TBoolean::subtract(skx::VariableValue *source) {
    return VariableValue::subtract(source);
}

bool skx::TBoolean::multiply(skx::VariableValue *source) {
    return VariableValue::multiply(source);
}

bool skx::TBoolean::divide(skx::VariableValue *source) {
    return VariableValue::divide(source);
}

skx::VariableValue *skx::TBoolean::copyValue() {
    return new TBoolean(value);
}
