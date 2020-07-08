//
// Created by liz3 on 29/06/2020.
//

#ifndef SKX_VARIABLE_H
#define SKX_VARIABLE_H

#include "Context.h"
#include <string>


namespace skx {
class Context;
    enum VarType {
        STRING,
        NUMBER,
        ARRAY,
        CHARACTER,
        POINTER,
        BOOLEAN,
        UNDEFINED
    };

    enum AccessType {
        STATIC,
        CONTEXT,
        GLOBAL
    };
    enum VarState {
        SPOILED,
        RUNTIME_CR
    };
    struct ValueDescriptor {
        std::string name;
        AccessType type;

    };

    class Variable {
    public:
        AccessType accessType;
        VarType type;
        Context* ctx;
        std::string name;
        void* value;
        bool isDouble = false; // specific to number;
        static void createVarFromOption(std::string item, skx::Context* targetContext, bool isStatic);
        static ValueDescriptor* extractNameSafe(std::string in);
        VarState state = SPOILED;

    };
}


#endif //SKX_VARIABLE_H
