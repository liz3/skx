//
// Created by liz3 on 16/07/2020.
//

#ifndef SKX_ASSEMBLER_H
#define SKX_ASSEMBLER_H


#include "Script.h"
#include <fstream>

namespace skx {
    class Assembler {
    public:
        static void assemble(Script* script);

    private:
        static void assembleTree(CompileItem* item, uint32_t counter, std::ofstream *stream, Function* function);
        static void assembleTrigger(Trigger* trigger, std::ofstream* stream);
        static void assembleAssigment(Assigment *assigment, std::ofstream *stream);
        static uint8_t getVarIdFromValue(VariableValue* value);
        static uint8_t writeValue(VariableValue* value, std::ofstream* stream);

        static void assembleComparison(Comparison *&comparison, std::ofstream *pOfstream);

        static void assembleOptions(Context *pContext, std::ofstream* ofstream);

        static void assembleExecution(Execution *&execution, std::ofstream *stream);
    };
}


#endif //SKX_ASSEMBLER_H
