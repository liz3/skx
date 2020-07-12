//
// Created by liz3 on 29/06/2020.
//

#ifndef SKX_SCRIPT_H
#define SKX_SCRIPT_H

#include "Context.h"
#include "../include/PreParser.h"
#include "../include/TreeCompiler.h"

#include <vector>
#include <map>


namespace skx {
    class Script {
    public:
        virtual ~Script();
        //base context holding functions & options vars and so on
        Context* baseContext;
        //self explaining
        PreParseResult* preParseResult;

        //actual compiled trees
        std::map<TriggerSignal*, CompileItem*> signals;
        std::map<TriggerCommand*, CompileItem*> mc_commands;
        std::map<TriggerEvent*, CompileItem*> mc_events;

        static Script* parse(const char* input);
        Script(Context *baseContext);
    };
}


#endif //SKX_SCRIPT_H
