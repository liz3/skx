//
// Created by liz3 on 10/07/2020.
//

#include "../../include/types/TNumber.h"

skx::TNumber::TNumber() {
    type = NUMBER;
}

skx::TNumber::TNumber(int32_t intValue) : intValue(intValue) {
    this->type = NUMBER;
}

skx::TNumber::TNumber(double doubleValue) : doubleValue(doubleValue) {
    isDouble = true;
    type = NUMBER;
}

bool skx::TNumber::isEqual(skx::VariableValue *other) {
    TNumber *val = dynamic_cast<TNumber *>(other);
    if (isDouble && val->isDouble) {
        return doubleValue == val->doubleValue;
    }
    if (isDouble) {
        return doubleValue == val->intValue;
    }
    if (val->isDouble) {
        return intValue == val->doubleValue;
    }
    return intValue == val->intValue;
}

bool skx::TNumber::isSmaller(skx::VariableValue *other) {
    TNumber *val = dynamic_cast<TNumber *>(other);
    if (isDouble && val->isDouble) {
        return doubleValue < val->doubleValue;
    }
    if (isDouble) {
        return doubleValue < val->intValue;
    }
    if (val->isDouble) {
        return intValue < val->doubleValue;
    }
    return intValue < val->intValue;
}

bool skx::TNumber::isBigger(skx::VariableValue *other) {
    TNumber *val = dynamic_cast<TNumber *>(other);
    if (isDouble && val->isDouble) {
        return doubleValue > val->doubleValue;
    }
    if (isDouble) {
        return doubleValue > val->intValue;
    }
    if (val->isDouble) {
        return intValue > val->doubleValue;
    }
    return intValue > val->intValue;
}

bool skx::TNumber::isBiggerOrEquals(skx::VariableValue *other) {
    TNumber *val = dynamic_cast<TNumber *>(other);
    if (isDouble && val->isDouble) {
        return doubleValue >= val->doubleValue;
    }
    if (isDouble) {
        return doubleValue >= val->intValue;
    }
    if (val->isDouble) {
        return intValue >= val->doubleValue;
    }
    return intValue >= val->intValue;
}

bool skx::TNumber::isSmallerOrEquals(skx::VariableValue *other) {
    TNumber *val = dynamic_cast<TNumber *>(other);
    if (isDouble && val->isDouble) {
        return doubleValue <= val->doubleValue;
    }
    if (isDouble) {
        return doubleValue <= val->intValue;
    }
    if (val->isDouble) {
        return intValue <= val->doubleValue;
    }
    return intValue <= val->intValue;
}

bool skx::TNumber::assign(skx::VariableValue *source) {
    if(isReadOnly) return false;
    TNumber *val = dynamic_cast<TNumber *>(source);
    if(isDouble) {
        doubleValue = val->isDouble ? val->doubleValue : val->intValue;
    } else {
        intValue = val->isDouble ? val->doubleValue : val->intValue;

    }
    return true;
}

bool skx::TNumber::add(skx::VariableValue *source) {
    if(isReadOnly) return false;
    TNumber *val = dynamic_cast<TNumber *>(source);
    if(isDouble) {
        doubleValue += (val->isDouble ? (val->doubleValue) : val->intValue);
    } else {
        intValue += (val->isDouble ? val->doubleValue : val->intValue);

    }
    return true;
}

bool skx::TNumber::subtract(skx::VariableValue *source) {
    if(isReadOnly) return false;
    TNumber *val = dynamic_cast<TNumber *>(source);
    if(isDouble) {
        doubleValue -= (val->isDouble ? (val->doubleValue) : val->intValue);
    } else {
        intValue -= (val->isDouble ? val->doubleValue : val->intValue);

    }
    return true;
}

bool skx::TNumber::divide(skx::VariableValue *source) {
    if(isReadOnly) return false;
    TNumber *val = dynamic_cast<TNumber *>(source);
    if(isDouble) {
        doubleValue = doubleValue / (val->isDouble ? (val->doubleValue) : val->intValue);
    } else {
        intValue = intValue / (val->isDouble ? val->doubleValue : val->intValue);

    }
    return true;
}

bool skx::TNumber::multiply(skx::VariableValue *source) {
    if(isReadOnly) return false;
    TNumber *val = dynamic_cast<TNumber *>(source);
    if(isDouble) {
        doubleValue = doubleValue * (val->isDouble ? (val->doubleValue) : val->intValue);
    } else {
        intValue = intValue * (val->isDouble ? val->doubleValue : val->intValue);

    }
    return true;
}

skx::VariableValue *skx::TNumber::copyValue() {
    return new TNumber(isDouble ? doubleValue : intValue);
}
