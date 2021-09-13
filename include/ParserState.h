#ifndef SKX_PARSERSTATE_H
#define SKX_PARSERSTATE_H

#include <string>
#include "Variable.h"
#include "Literal.h"
#include "TreeCompiler.h"
#include "Instruction.h"
#include "BasicInstructions.h"
#include "Context.h"

namespace skx {
enum ParserSuperType {
  IF_STATEMENT,
ELSE_IF_STATEMENT,
  ASSIGMENT,
  FUNCTION_INVOKE,
  COMPLEX_STATEMENT,
  OTHER,
  LOOP_STMT,
  RETURN_STMT,
  MATH_OPERATION
};
enum ParserContextState {
  ENTRY,
  DEFINITION,
  STRING_LITERAL,
  NUMBER_LITERAL,
  BOOLEAN_LITERAL,
  FUNCTION_CALL,
  OPERATOR,

};
struct OperatorResult {
 InstructionOperator op;
 bool success;
 bool inverted;
};
class __declspec(dllexport) ParserState {
public:
  ParserState(std::string content);
  uint16_t nextWhiteSpace();
  void skip(uint16_t amount);
  std::string consume(uint16_t amount);
  bool isWhiteSpace(char content);
  uint16_t nextNonWhiteSpace();
  bool nextIs(ParserContextState what);
  VariableDescriptor* getNextAsVariable();
  char next();
  bool isOperator(std::string in);
  std::string getNextAsString();
  OperatorPart* getNextAsNumber();
  OperatorPart* getNextAsBoolean();
  OperatorResult getNextAsOperator();
  FunctionInvoker* getNextAsFunctionInvoke(Context* ctx);
  OperatorPart* getNextAsValue(Context* ctx);
private:
  void setup();
  bool isNumber(char c);
  std::string content;
  uint16_t pos;
  ParserSuperType type;
  ParserContextState currentState;
  std::string mathOperation;
};

} // namespace skx


#endif