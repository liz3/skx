//
// Created by Liz3 on 05/06/2021.
//

#ifndef SKX_NATIVECALL_H
#define SKX_NATIVECALL_H
#include "../Instruction.h"
#include "../TreeCompiler.h"
namespace skx {

class NativeCallInterface : public Execution {
 public:
  enum CallType {
    UNKNOWN,
    READFILE,
    WRITEFILE,
    REMFILE,
    STRLEN,
    STRING_SPLIT,
    MAP_SIZE,

    MATH_POW,
    MATH_SIN,
    MATH_COS,
    MATH_TAN,
    MATH_SQRT,
    MATH_FLOOR,
    MATH_CEIL,
    GETENV,
    EVAL,
    WRITE_OUT
  };
  NativeCallInterface(): Execution() {
    name = "native::callinterface";
  };
  CallType type;
  OperatorPart* execute(Context* target) override;
};
class NativeCallCompiler {
 private:
  static NativeCallInterface::CallType getCallType(std::string& entry);
 public:
  static OperatorPart* compileCall(std::string& content, Context* pContext, CompileItem* pItem);
};
}

#endif
