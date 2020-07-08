//
// Created by liz3 on 30/06/2020.
//

#ifndef SKX_INSTRUCTION_H
#define SKX_INSTRUCTION_H

#include <vector>
#include "Variable.h"

namespace skx {
    enum InstructionOperator {
        EQUAL,
        NOT_EQUAL,
        SMALLER,
        BIGGER,
        SMALLER_OR_EQUAL,
        BIGGER_OR_EQUAL,
        //Only for assigning
        ASSIGN,
        SUBTRACT,
        ADD,
        MULTIPLY,
        DIVIDE,
        INVERT,
        NOOP
    };
    enum OperatorType {
        LITERAL,
        VARIABLE,
        EXECUTION,
        DESCRIPTOR
    };
    enum TriggerType {
        EVENT,
        MC_COMMAND,
        BOOT

    };
    struct OperatorPart {
        OperatorType operatorType;
        VarType type;
        void* value = nullptr;
        bool isDouble = false;

        virtual ~OperatorPart();

        OperatorPart(OperatorType operatorType, VarType type, void *value, bool isDouble);
    };
    class Comparison {
    public:
        virtual ~Comparison();

        OperatorPart* source;
       OperatorPart* target;
       InstructionOperator type;
       bool execute(Context* context);

    };
    class ReturnOperation {
    public:
        OperatorPart* targetReturnItem = nullptr;
    };
    class Assigment {
    public:
        virtual ~Assigment();

        OperatorPart* source;
        OperatorPart* target;
        InstructionOperator type;
        bool execute(Context* context);

    };
    class Execution {
    public:
        virtual ~Execution();

        std::string name;
        std::vector<OperatorPart*> dependencies;
        virtual OperatorPart* execute(Context* target);
    };
    class Trigger {
    public:
        TriggerType type;
    };


}


#endif //SKX_INSTRUCTION_H
