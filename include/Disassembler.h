//
// Created by liz3 on 16/07/2020.
//

#ifndef SKX_DISASSEMBLER_H
#define SKX_DISASSEMBLER_H

#include <fstream>
#include "Script.h"

namespace skx {
    class Disassembler {
    public:
        static Script* parse();

    private:
        static std::string readString(std::ifstream * stream);
        static void parseOptions(std::ifstream *stream, Context *ctx);
        static VarType getType(uint8_t val);
        static VariableValue* readValue(std::ifstream* stream, VarType type, bool isDouble);
        static void parseTree(Context* ctx, std::ifstream* stream, uint32_t counter, CompileItem* parent);
    };
}


#endif //SKX_DISASSEMBLER_H
