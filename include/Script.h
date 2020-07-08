//
// Created by liz3 on 29/06/2020.
//

#ifndef SKX_SCRIPT_H
#define SKX_SCRIPT_H

#include "Context.h"
#include "../include/PreParser.h"
#include "../include/TreeCompiler.h"

#include <vector>


namespace skx {
    class Script {
    public:
        virtual ~Script();

        Context* baseContext;
        PreParseResult* preParseResult;
        std::vector<CompileItem*> compiledPreRuntimeEvents;
        void walk(PreParserItem *item, Context *itemContext, Script *script);
        static Script* parse(char* input);
        Script(Context *baseContext);
    };
}


#endif //SKX_SCRIPT_H
