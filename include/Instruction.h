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
    };
    enum OperatorType {
        LITERAL,
        VARIABLE,
        EXECUTION,
    };
    enum TriggerType {
        MC_EVENT,
        MC_COMMAND,
        SIGNAL

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
        virtual ~Trigger();

        TriggerType type;
    };

    class TriggerCommand : public Trigger {
    public:
        TriggerCommand() : Trigger() {
          type = MC_COMMAND;
        }
        std::string name;
        // more to come
    };
    class TriggerEvent : public Trigger {
    public:
        TriggerEvent() : Trigger() {
            type = MC_EVENT;
        }
        std::string eventClass = "org.bukkit.event.player.PlayerJoinEvent";

    };
    class TriggerSignal : public Trigger {
    public:
        TriggerSignal() : Trigger() {
            type = SIGNAL;
        }

        virtual ~TriggerSignal() {

        }
        enum TriggerTypeSignal {
            LOAD,
            UN_LOAD
        };
        TriggerTypeSignal signalType;
    };

}


#endif //SKX_INSTRUCTION_H
