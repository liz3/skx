//
// Created by liz3 on 29/06/2020.
//

#ifndef SKX_BASETYPEPARSER_H
#define SKX_BASETYPEPARSER_H

#include <string>

namespace skx {
    enum BaseActionType {
        OPTIONS,
        PRE_RUNTIME_EVENT,
        RUNTIME_EVENT,
        POST_RUNTIME_EVENT, // No idea if this will be used
        COMMAND,
        FUNCTION,
        SCHEDULED_ACTION,
        UNKNOWN
        };
    class BaseAction {
    public:
        static BaseActionType getBaseType(std::string item) {
            if(item.rfind("function", 0) == 0) return FUNCTION;
            if(item.rfind("options", 0) == 0) return OPTIONS;
            if(item.rfind("command", 0) == 0) return COMMAND;
            if(item.rfind("every", 0) == 0) return SCHEDULED_ACTION;

            if(item.rfind("on load", 0) == 0) return PRE_RUNTIME_EVENT;
            if(item.rfind("on", 0) == 0) return RUNTIME_EVENT;

            return RUNTIME_EVENT;
        }
    };
}

#endif //SKX_BASETYPEPARSER_H
