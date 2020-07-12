//
// Created by liz3 on 10/07/2020.
//

#ifndef SKX_JSON_H
#define SKX_JSON_H

#include "../Instruction.h"
#include "../TreeCompiler.h"
#include "../../third-party/json/single_include/nlohmann/json.hpp"

namespace skx {
    class TJson : public VariableValue {
    public:
        TJson(const nlohmann::json &value);
        TJson();

        virtual ~TJson();

        nlohmann::json value;
        VariableValue * copyValue() override;
    };
    class JsonInterface : public Execution {
    public:
        enum ActionType {
            PARSE,
            STRINGIFY,
            PUT,
            REMOVE,
            CREATE_OBJECT,
            CREATE_ARRAY,
            EXTRACT
        };

        JsonInterface(): Execution() {
            name = "data::json";
        }
        ActionType type;
        OperatorPart * execute(Context *target) override;
    };
    class Json {
    public:
        static void compileRequest(std::string& content, Context *pContext, CompileItem *pItem);
    };
}


#endif //SKX_JSON_H
