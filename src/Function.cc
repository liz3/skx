//
// Created by liz3 on 29/06/2020.
//

#include "../include/Function.h"
#include "../include/Executor.h"

skx::OperatorPart *skx::Function::run(std::vector<OperatorPart *> execVars, Context *callingContext) {
  if (execVars.size() != targetParams.size())
    return nullptr; //param len mismatch
  OperatorPart *returnValue = nullptr;
  //setup stack
  for (uint32_t i = 0; i < targetParams.size(); ++i) {
    auto currentTarget = targetParams[i];
    auto targetVar = execVars[i];
    if (targetVar->operatorType == VARIABLE) {
      Variable *tVar = static_cast<Variable *>(targetVar->value);
      Variable *funcVar = functionItem->ctx->vars[currentTarget->name];
      skx::Utils::copyVariableValue(tVar, funcVar);
    } else if (targetVar->operatorType == LITERAL) {
      Variable *funcVar = functionItem->ctx->vars[currentTarget->name];
      if(funcVar->getValue() == nullptr) {
        funcVar->type = targetVar->type;
        funcVar->setValue(static_cast<VariableValue*>( targetVar->value));
      }
    }
  }
  skx::Executor executor;
  auto* result = executor.executeFunction(functionItem);
  return result;
}

skx::Function::~Function() {
  for (auto & targetParam : targetParams) {
    delete targetParam;
  }
  targetParams.clear();
  delete functionItem;
}
