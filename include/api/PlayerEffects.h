//
// Created by liz3 on 16/07/2020.
//

#ifndef SKX_PLAYEREFFECTS_H
#define SKX_PLAYEREFFECTS_H

#include "../Instruction.h"
#include "../TreeCompiler.h"

namespace skx {
    class PlayerAction : public Execution {
        enum ActionType {
            KICK,
            BAN,
            SEND_MESSAGE
        };
    public:
        PlayerAction() : Execution() {
            name = "mc::player::action";
        }

        virtual ~PlayerAction();
        static Execution* compilePlayerInstruction(std::string& content, Context *pContext, CompileItem *pItem);
        TriggerEvent* ref = nullptr;
        TriggerCommand* commandRef = nullptr;
        ActionType targetType;
        OperatorPart * execute(Context *target) override;
    };
}


#endif //SKX_PLAYEREFFECTS_H
