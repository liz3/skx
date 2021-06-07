//
// Created by liz3 on 30/06/2020.
//

#ifndef SKX_EXECUTOR_H
#define SKX_EXECUTOR_H

#include "TreeCompiler.h"

namespace skx {
#ifdef _WIN32
class  __declspec(dllexport) Executor {
#else
class Executor {
#endif
 public:

  static void executeStandalone(CompileItem* item);
  void execute(CompileItem* item);
  bool stopLoop = false;
  CompileItem* root = nullptr;
  void walk(CompileItem* item);
  bool lastFailed = false;
  bool isLoop = false;
  uint16_t lastFailLevel = 0;
};
}

#endif //SKX_EXECUTOR_H
