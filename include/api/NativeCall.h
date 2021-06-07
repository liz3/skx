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
    STRLEN,
    STRING_SPLIT,
    GETENV
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
