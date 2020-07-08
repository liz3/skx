//
// Created by liz3 on 29/06/2020.
//

#ifndef SKX_CONTEXT_H
#define SKX_CONTEXT_H


#include <cstdint>
#include <map>
#include <vector>
#include <string>


namespace skx {
class Variable;
class Function;
    class Context {
    public:
        virtual ~Context();

        uint16_t level;
        uint32_t steps;
        uint32_t stepPointer;
        Context* parent = nullptr;
        Context* global = nullptr; // ROOT context, has global vars and shit like that that
        std::map<std::string, skx::Variable*> vars;
        std::map<std::string, skx::Function*> functions;
        std::vector<Context*> childContexts;

        Context(uint16_t level, uint32_t steps, uint32_t stepPointer, Context *parent);
    };
}


#endif //SKX_CONTEXT_H
