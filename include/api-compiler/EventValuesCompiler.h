//
// Created by Yann Holme Nielsen on 07/08/2020.
//

#ifndef SKX_EVENTVALUESCOMPILER_H
#define SKX_EVENTVALUESCOMPILER_H

#include <string>
#include "../Context.h"
#include "../TreeCompiler.h"

namespace skx {
    class EventValuesCompiler {
    public:
        static OperatorPart* compile(std::string &content, skx::Context *ctx, skx::CompileItem *target,
                            bool isElseIf);
    };
}


#endif //SKX_EVENTVALUESCOMPILER_H
