//
// Created by liz3 on 10/07/2020.
//

#include "../../include/types/TArray.h"

bool skx::TArray::assign(skx::VariableValue *source) {
    TArray* val = dynamic_cast<TArray*>(source);
    entries.clear();
    entries = std::vector<VariableValue*>(val->entries);
    return true;
}

bool skx::TArray::isEqual(skx::VariableValue *other) {
    TArray* val = dynamic_cast<TArray*>(other);
    return entries.size() == val->entries.size();
}

bool skx::TArray::isSmaller(skx::VariableValue *other) {
    TArray* val = dynamic_cast<TArray*>(other);
    return entries.size() < val->entries.size();
}

bool skx::TArray::isBigger(skx::VariableValue *other) {
    TArray* val = dynamic_cast<TArray*>(other);
    return entries.size() > val->entries.size();
}

bool skx::TArray::isSmallerOrEquals(skx::VariableValue *other) {
    TArray* val = dynamic_cast<TArray*>(other);
    return entries.size() <= val->entries.size();
}

bool skx::TArray::isBiggerOrEquals(skx::VariableValue *other) {
    TArray* val = dynamic_cast<TArray*>(other);
    return entries.size() >= val->entries.size();
}

bool skx::TArray::add(skx::VariableValue *source) {
    entries.push_back(source);
    return true;
}

skx::TArray::TArray() {
    type = ARRAY;
}

skx::VariableValue *skx::TArray::copyValue() {
    TArray* arr = new TArray();
    arr->entries = std::vector<VariableValue*>(entries);
    return arr;
}
