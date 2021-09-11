//
// Created by liz3 on 29/06/2020.
//

#ifndef SKX_FUNCTION_H
#define SKX_FUNCTION_H

#include "Variable.h"
#include "PreParser.h"
#include "TreeCompiler.h"
#include "Context.h"
#include "utils.h"
#include <vector>
#include <string>

namespace skx {
class Function {
 private:
 public:
  virtual ~Function();

  std::string name;
  std::vector<VariableDescriptor*> targetParams;
  OperatorPart *run(std::vector<OperatorPart *> execVars, Context *callingContext);
  CompileItem* functionItem = nullptr;
  VarType returnType = UNDEFINED;

};
}

#endif //SKX_FUNCTION_H
