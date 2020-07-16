//
// Created by liz3 on 16/07/2020.
//

#include "../include/Disassembler.h"
#include "../include/types/TString.h"
#include "../include/types/TBoolean.h"
#include "../include/types/TNumber.h"
#include "../include/types/TCharacter.h"
#include "../include/api/Json.h"
#include <nlohmann/json.hpp>


skx::Script *skx::Disassembler::parse() {
    std::ifstream stream("out.skx");
    stream.seekg(5);
    uint32_t size = 0;
    stream.read(reinterpret_cast<char *>(&size), 4);
    Script* result = new Script(new Context(0, size, 0, nullptr));
    parseOptions(&stream, result->baseContext);
    for (int i = 0; i < size; ++i) {
        uint8_t superType;
        stream.read(reinterpret_cast<char *>(&superType), 1);

    }
    return nullptr;
}

void skx::Disassembler::parseOptions(std::ifstream *stream, skx::Context *ctx) {
    uint8_t type = 0;
    stream->read(reinterpret_cast<char *>(&type), 1);
    if(type != 2) return;
    uint32_t count = 0;
    stream->read(reinterpret_cast<char *>(&count), 4);
    for (int i = 0; i < count; ++i) {
        std::string name = readString(stream);
        uint8_t varTypeNum = 0;
        stream->read(reinterpret_cast<char *>(&varTypeNum), 1);
        VarType varType = getType(varTypeNum);
        VariableValue* val = readValue(stream, varType, varTypeNum == 3);
        if(val == nullptr) continue;
        Variable* var = val->varRef ? val->varRef : new Variable();
        if(!val->varRef) var->setValue(val);
        var->contextValue = false;
        var->accessType = STATIC;
        var->ctx = ctx;
        var->name = name;
        ctx->vars[name] = var;
    }
}

std::string skx::Disassembler::readString(std::ifstream *stream) {
    std::vector<char> chars;
    while (true) {
        char x = stream->get();
        chars.push_back(x);
        if(x == '\0') {
            break;
        }
    }
    return std::string(&chars[0]);
}

skx::VarType skx::Disassembler::getType(uint8_t val) {
    switch (val) {
        case 0:
            return STRING;
        case 1:
            return BOOLEAN;
        case 2:
        case 3:
            return NUMBER;
        case 4:
            return CHARACTER;
        case 5:
            return POINTER;
        case 6:
            return UNDEFINED;
        case 7:
            return ARRAY;
        default:
            break;
    }
    return skx::NUMBER;
}

skx::VariableValue *skx::Disassembler::readValue(std::ifstream *stream, skx::VarType type, bool isDouble) {
    switch (type) {
        case STRING:
            return new TString(readString(stream));
        case BOOLEAN: {
            uint8_t val;
            stream->read(reinterpret_cast<char *>(&val), 1);
            return new TBoolean(val == 1);
        }
        case NUMBER: {
            if(isDouble) {
                double value;
                stream->read(reinterpret_cast<char *>(&value), 8);
                return new TNumber(value);
            } else {
                int32_t value;
                stream->read(reinterpret_cast<char *>(&value), 4);
                return new TNumber(value);
            }
        }
        case CHARACTER: {
            char x;
            stream->read(reinterpret_cast<char *>(&x), 1);
            return new TCharacter(x);
        }
        case POINTER: {
            uint8_t val;
            stream->read(reinterpret_cast<char *>(&val), 1);
            if(val == 0) return nullptr;
            std::string typeValue = readString(stream);
            std::string stringVal = readString(stream);
            if(typeValue == "data::json") {
                Variable* var = new Variable();
                var->customTypeName = typeValue;
                TJson* jsonVal =new TJson(nlohmann::json::parse(stringVal));
                var->setValue(jsonVal);
                return jsonVal;
            }
        }
    }
    return nullptr;
}

void
skx::Disassembler::parseTree(skx::Context *ctx, std::ifstream *stream, uint32_t counter, skx::CompileItem *parent) {

}
