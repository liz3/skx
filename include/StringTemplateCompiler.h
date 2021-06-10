//
// Created by Liz3 on 10/06/2021
//
#ifndef SKX_STRINGTEMPLATECOMPILER_H
#define SKX_STRINGTEMPLATECOMPILER_H

#include "Instruction.h"
#include "Variable.h"
#include "TreeCompiler.h"
#include "BasicInstructions.h"
#include "utils.h"

namespace skx {
  class StringTemplateCompiler {
public:
  static OperatorPart* compile(std::string baseStr, Context* ctx, CompileItem* target);
  };

}

#endif
