//
// Created by liz3 on 29/06/2020.
//

#include "../include/Variable.h"
#include "../include/utils.h"
#include "../include/types/TBoolean.h"
#include "../include/types/TString.h"
#include "../include/types/TNumber.h"
#include "../include/types/TCharacter.h"
#include "../include/types/TArray.h"
#include "../include/api/Json.h"
#include "../include/types/TMap.h"


void skx::Variable::createVarFromOption(std::string item, skx::Context *targetContext) {
    auto start = item.find_first_of(':') + 1;
    std::string name = skx::Utils::trim(item.substr(0, start - 1));
    std::string value = skx::Utils::trim(item.substr(start, item.length() - 1));
    skx::Variable *variable = new Variable();
    if (value == "true") {
        variable->type = BOOLEAN;
        variable->value = new TBoolean(true);
        variable->value->varRef = variable;

    } else if (value == "false") {
        variable->type = BOOLEAN;
        variable->value = new TBoolean(false);
        variable->value->varRef = variable;
    } else {
        variable->type = STRING;
        variable->value = new TString(value);
        variable->value->varRef = variable;

    }
    variable->ctx = targetContext;
    variable->name = name;
    variable->accessType = STATIC;
    targetContext->vars[name] = variable;
}

skx::VariableDescriptor *skx::Variable::extractNameSafe(std::string in) {
    auto *descriptor = new VariableDescriptor();
    descriptor->isFromContext = in[0] == '%' && in[in.length()-1] == '%';
    auto trim = in.substr(1, in.length() - 2);
    if (trim.rfind('@', 0) == 0) {
        descriptor->type = STATIC;
        trim = trim.substr(1);
    } else if (trim.rfind('_', 0) == 0) {
        descriptor->type = CONTEXT;
        trim = trim.substr(1);
    } else {
        descriptor->type = descriptor->isFromContext ? CONTEXT : GLOBAL;
    }
    if(trim.find("::") != std::string::npos) {
        descriptor->name = trim.substr(0, trim.find("::"));
        std::string accessorLiteral = trim.substr(trim.find("::") + 2);
        ListAccessor* accessor = new ListAccessor();
        if(accessorLiteral == "*") {
            accessor->type = ALL;
        } else {
            if((accessorLiteral[0] == '%' && accessorLiteral[accessorLiteral.length() -1] == '%') ||
                    (accessorLiteral[0] == '{' && accessorLiteral[accessorLiteral.length() -1] == '}')) {
                accessor->name = accessorLiteral.substr(1, accessorLiteral.length() -2);
                accessor->type = VAR_VALUE;
            }
        }

        descriptor->listAccessor = accessor;
    } else {

    descriptor->name = trim;
    }
    return descriptor;
}

skx::Variable::~Variable() {
    if(value != nullptr && value->varRef == this)
        delete value;
//    if (value != nullptr && !created) {
//        switch (type) {
//            case STRING: {
//                auto *val = dynamic_cast<TString*>(value);
//
//                delete val;
//                break;
//            }
//            case NUMBER: {
//                auto *val = dynamic_cast<TNumber*>(value);
//                delete val;
//                break;
//            }
//            case CHARACTER: {
//                auto *val = dynamic_cast<TCharacter*>(value);
//                delete val;
//                break;
//            }
//            case BOOLEAN: {
//                auto *val = dynamic_cast<TBoolean*>(value);
//                delete val;
//                break;
//            }
//            default:
//                delete value;
//                break;
//        }
//    }
    value = nullptr;
}

void skx::Variable::createVarValue(VarType type, Variable* target, bool isDouble) {
    target->type = type;
    switch (type) {
        case STRING:
            target->setValue( new TString());
            break;
        case NUMBER: {
            TNumber* v = new TNumber();
            v->isDouble = isDouble;
            target->setValue(v);
        }
            break;
        case ARRAY:
            target->setValue(new TArray());
            break;
        case CHARACTER:
            target->setValue(new TCharacter());
            break;
        case BOOLEAN:
            target->setValue(new TBoolean());
            break;
        case POINTER:
            target->setValue(nullptr);
            break;
        default:
            break;
    }
}

skx::VariableValue *skx::Variable::getValue() const {
    return value;
}

void skx::Variable::setValue(skx::VariableValue *value) {
    if(this->getValue() != nullptr && this->getValue()->varRef == this) {
        delete this->getValue();
        this->value = nullptr;
    }
    this->value = value;
    if(value != nullptr) {
        if(value->varRef != nullptr) {
            this->customTypeName = value->varRef->customTypeName;
        }
        if(value->varRef == nullptr)
            this->value->varRef = this;
    }
}

skx::VariableValue *skx::Variable::extractValueFromList(Variable* map, Variable* index) {
    if(map->type != MAP) return nullptr;
    if(index->getValue()  == nullptr) return nullptr;
    std::string search = index->getValue()->getStringValue();
    TMap* iteratorMap = dynamic_cast<TMap *>(map->getValue());
    if(iteratorMap->keyMap.find(search) != iteratorMap->keyMap.end())
        return iteratorMap->keyMap[search];
    return nullptr;
}

bool skx::VariableValue::isEqual(skx::VariableValue *other) {
    return false;
}

bool skx::VariableValue::isSmaller(skx::VariableValue *other) {
    return false;
}

bool skx::VariableValue::isBigger(skx::VariableValue *other) {
    return false;
}

bool skx::VariableValue::isSmallerOrEquals(skx::VariableValue *other) {
    return false;
}

bool skx::VariableValue::isBiggerOrEquals(skx::VariableValue *other) {
    return false;
}

bool skx::VariableValue::assign(skx::VariableValue *source) {
    return false;
}

bool skx::VariableValue::subtract(skx::VariableValue *source) {
    return false;
}

bool skx::VariableValue::add(skx::VariableValue *source) {
    return false;
}

bool skx::VariableValue::multiply(skx::VariableValue *source) {
    return false;
}

bool skx::VariableValue::divide(skx::VariableValue *source) {
    return false;
}

skx::VariableValue::~VariableValue() {

}

skx::VariableValue *skx::VariableValue::copyValue() {
    return nullptr;
}

std::string skx::VariableValue::getStringValue() {
    return std::string();
}
