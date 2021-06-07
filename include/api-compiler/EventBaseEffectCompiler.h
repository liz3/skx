//
// Created by Yann Holme Nielsen on 07/08/2020.
//

#ifndef SKX_EVENTBASEEFFECTCOMPILER_H
#define SKX_EVENTBASEEFFECTCOMPILER_H


#include <string>
#include "../Context.h"
#include "../TreeCompiler.h"

namespace skx {
class EventBaseEffectCompiler {
 public:
  static void compile(std::string &content, skx::Context *ctx, skx::CompileItem *target);
};
}


#endif //SKX_EVENTBASEEFFECTCOMPILER_H
