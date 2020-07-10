//
// Created by liz3 on 10/07/2020.
//

#include "../../include/api/Http.h"

skx::OperatorPart *skx::HttpInterface::execute(skx::Context *target) {
    return Execution::execute(target);
}

void skx::Http::compileRequest(std::string &content, skx::Context *pContext, skx::CompileItem *pItem) {
    HttpInterface* execution = new HttpInterface();



}

skx::Http::RequestType skx::Http::getTypeFromString(std::string &what) {
    return skx::Http::OPTIONS;
}
