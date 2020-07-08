//
// Created by liz3 on 29/06/2020.
//

#ifndef SKX_FUNCTION_H
#define SKX_FUNCTION_H

#include "Variable.h"
#include "Step.h"
#include "PreParser.h"
#include "TreeCompiler.h"
#include "Context.h"
#include "utils.h"
#include <vector>
#include <string>

namespace skx {
    struct ReturnOpWithCtx {
        Context* ctx;
        ReturnOperation* descriptor;
    };
    class Function {
    private:
        ReturnOpWithCtx* walk(CompileItem* item);
        bool lastFailed = false;
        uint16_t lastFailLevel = 0;
    public:
        std::string name;
        std::vector<VariableDescriptor*> targetParams;
        OperatorPart *run(std::vector<OperatorPart *> execVars, Context *callingContext);
        CompileItem* functionItem;
        VarType returnType = UNDEFINED;

    };
}

#endif //SKX_FUNCTION_H
