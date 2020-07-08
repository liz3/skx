//
// Created by liz3 on 01/07/2020.
//

#ifndef SKX_STDLIB_H
#define SKX_STDLIB_H

#include "Instruction.h"
#include "Variable.h"


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
}

#endif //SKX_STDLIB_H
