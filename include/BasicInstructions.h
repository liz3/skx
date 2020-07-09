//
// Created by liz3 on 01/07/2020.
//

#ifndef SKX_BASICINSTRUCTIONS_H
#define SKX_BASICINSTRUCTIONS_H

#include "Instruction.h"
#include "Variable.h"
#include "TreeCompiler.h"


namespace skx {
    class Print : public Execution {
    public:
        Print(): Execution() {
            name = "std::print";
        }

        OperatorPart* execute(Context *target) override;
    };
    class FunctionInvoker : public Execution { //Yep, this is a execution
    public:
        FunctionInvoker() : Execution() {
            name = "std::invoke";
        }
        Function* function;
        OperatorPart* execute(Context *target) override;
    };
    class Loop : public Execution {
    public:
        Loop() : Execution() {
            name = "std::loop";
        }
        int32_t loopTarget = 0;
        Variable* loopTargetVar = nullptr;
        CompileItem* rootItem;
        bool hasCondition = false;
        Comparison* comparison = nullptr;
        Variable* loopCounter = nullptr;
        OperatorPart* execute(Context *target) override;
    };
}

#endif //SKX_BASICINSTRUCTIONS_H
