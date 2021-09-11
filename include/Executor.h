//
// Created by liz3 on 30/06/2020.
//

#ifndef SKX_EXECUTOR_H
#define SKX_EXECUTOR_H

#include "TreeCompiler.h"

namespace skx {
struct ReturnOpWithCtx {
  Context* ctx;
  ReturnOperation* descriptor;
};

#ifdef _WIN32
class  __declspec(dllexport) Executor {
#else
class Executor {
#endif
 public:

  static void executeStandalone(CompileItem* item);
  OperatorPart* execute(CompileItem* item);
  skx::OperatorPart* executeFunction(CompileItem* item);
  bool stopLoop = false;
  CompileItem* root = nullptr;
  ReturnOpWithCtx* walk(CompileItem* item);
  bool isFunction = false;
  bool lastFailed = false;
  bool isLoop = false;
  uint16_t lastFailLevel = 0;
};
}

#endif //SKX_EXECUTOR_H
