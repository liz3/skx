//
// Created by liz3 on 10/07/2020.
//

#ifndef SKX_HTTP_H
#define SKX_HTTP_H

#include "../Instruction.h"
#include "../TreeCompiler.h"

namespace skx {
class HttpInterface : public Execution {
 public:
  OperatorPart* body = nullptr;
  OperatorPart* url = nullptr;

  HttpInterface(): Execution() {
    name = "net::http";
  }

  OperatorPart * execute(Context *target) override;
};
class Http {
 public:
  enum RequestType {
    GET,
    POST,
    PATCH,
    DELETE,
    PUT,
    OPTIONS,
    HEAD
  };
  static void compileRequest(std::string& content, Context *pContext, CompileItem *pItem);
 private:
  RequestType getTypeFromString(std::string& what);
};
}


#endif //SKX_HTTP_H
