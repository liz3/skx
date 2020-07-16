//
// Created by liz3 on 29/06/2020.
//

#ifndef SKX_UTILS_H
#define SKX_UTILS_H

#include <vector>
#include <string>
#include "Variable.h"
#include "Instruction.h"

const std::string WHITESPACE = " \n\r\t\f\v";

namespace skx {
    class Utils {
    public:
        static std::string getEventClassFromExpression(std::string content);
        static std::string depListToString(std::vector<OperatorPart*> deps);
        static std::vector<std::string> split(std::string base, std::string delimiter);
        static Variable* searchRecursive(std::string what, Context* ctx);
        static void copyVariableValue(Variable* source, Variable* target);
        static void updateVarState(Context* ctx, VarState state);
        static Variable* searchVar(VariableDescriptor* descriptor, Context* ctx);

        static std::string ltrim(const std::string& s)
        {
            size_t start = s.find_first_not_of(WHITESPACE);
            return (start == std::string::npos) ? "" : s.substr(start);
        }

        static std::string rtrim(const std::string& s)
        {
            size_t end = s.find_last_not_of(WHITESPACE);
            return (end == std::string::npos) ? "" : s.substr(0, end + 1);
        }

        static std::string trim(const std::string& s)
        {
            return rtrim(ltrim(s));
        }


    };

}

#endif //SKX_UTILS_H
