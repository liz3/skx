//
// Created by liz3 on 10/07/2020.
//

#include "../../include/types/TCharacter.h"

bool skx::TCharacter::assign(skx::VariableValue *source) {
    if(isReadOnly) return false;
    TCharacter* val = dynamic_cast<TCharacter*>(source);
    value = val->value;
    return true;
}

bool skx::TCharacter::isEqual(skx::VariableValue *other) {
    TCharacter* val = dynamic_cast<TCharacter*>(other);
    return value == val->value;
}

bool skx::TCharacter::isBigger(skx::VariableValue *other) {
    TCharacter* val = dynamic_cast<TCharacter*>(other);
    return value > val->value;
}

bool skx::TCharacter::isSmaller(skx::VariableValue *other) {
    TCharacter* val = dynamic_cast<TCharacter*>(other);
    return value < val->value;
}

bool skx::TCharacter::isBiggerOrEquals(skx::VariableValue *other) {
    TCharacter* val = dynamic_cast<TCharacter*>(other);
    return value >= val->value;
}

bool skx::TCharacter::isSmallerOrEquals(skx::VariableValue *other) {
    TCharacter* val = dynamic_cast<TCharacter*>(other);
    return value= val->value;
}

skx::TCharacter::TCharacter(char value) : value(value) {
    type = CHARACTER;
}

skx::TCharacter::TCharacter() {}

skx::VariableValue *skx::TCharacter::copyValue() {
    return new TCharacter(value);
}
