//
// Created by liz3 on 30/06/2020.
//

#include "../include/TreeCompiler.h"
#include "../include/utils.h"
#include "../include/BasicInstructions.h"
#include "../include/Literal.h"
#include "../include/RegexUtils.h"

#include "../include/Function.h"
#include "../include/types/TNumber.h"
#include "../include/types/TString.h"
#include "../include/types/TBoolean.h"
#include "../include/api/Json.h"
#include "../include/api/NativeCall.h"
#include "../include/StringTemplateCompiler.h"
#include "../include/api/MapEffects.h"

#ifdef SKX_BUILD_API
#include "../include/api/McEventsBaseEffects.h"
#include "../include/api/RuntimeMcEventValues.h"
#include "../include/api/McEventPlayerEffects.h"
#include "../include/api/EventPreconditions.h"
#include "../include/api-compiler/EventValuesCompiler.h"
#include "../include/api-compiler/EventBaseEffectCompiler.h"
#include "../include/types/TPlayerRef.h"

#endif
#include <exception>
#include <iostream>

skx::CompileItem *skx::TreeCompiler::compileTree(skx::PreParserItem *item, skx::Context *ctx) {
  TreeCompiler compiler;
  CompileItem *root = new CompileItem();
  root->ctx = ctx;
  root->level = 0;
  root->root = root;
  compiler.compileExpression(item, ctx, root);
  compiler.advance(root, item);
  return root;
}

skx::CompileItem *skx::TreeCompiler::compileTreeFunction(skx::PreParserItem *item, skx::Context *ctx) {
  TreeCompiler compiler;
  compiler.isFunction = true;
  CompileItem *root = new CompileItem();
  Function *func = new Function();
  compiler.setupFunctionMeta(item->actualContent, func);
  Context *functionCtx = new Context(0, item->children.size(), 0, ctx);
  functionCtx->global = ctx->global;
  for (auto current : func->targetParams) {
    Variable *param = new Variable();
    param->name = current->name;
    param->accessType = current->type;
    param->setValue(nullptr);
    param->type = UNDEFINED;
    functionCtx->vars[param->name] = param;
  }
  ctx->functions[func->name] = func;
  root->root = root;
  root->ctx = functionCtx;
  compiler.advance(root, item, true);
  func->functionItem = root;
  return root;
}

void skx::TreeCompiler::compileExpression(skx::PreParserItem *item, Context *context, CompileItem *target) {

  if (item->level == 0) {
    compileTrigger(item->actualContent, context, target);
  } else {
    auto actualContent = item->actualContent;
    if (actualContent == "else:") {
      target->isElse = true;
      return;
    }
    if (actualContent == "break") {
      target->isBreak = true;
      return;
    }
    if (actualContent == "trigger:") {
      target->isCommandTrigger = true;
      return;
    }
    //ik this is messy, will change later
    if (actualContent.find("if") == 0) {
      compileCondition(actualContent, context, target);
    } else if (actualContent.find("else if") == 0) {
      compileCondition(actualContent, context, target);
    } else if (actualContent.find("set") == 0) {
      compileAssigment(actualContent, context, target);
    } else if (actualContent.find("loop") == 0) {
      compileLoop(actualContent, context, target);
    } else if (actualContent.find("add") == 0 || actualContent.find("subtract") == 0 ||
               actualContent.find("multiply") == 0 || actualContent.find("divide") == 0) {
      compileOperator(actualContent, context, target);
    } else if (actualContent.find("return") == 0) {
      compileReturn(actualContent, context, target);
    } else {
      compileExecution(actualContent, context, target);
    }
  }

}

void skx::TreeCompiler::advance(skx::CompileItem *parent, skx::PreParserItem *parentItem, bool isFuncSuperContext) {
  if (parentItem->children.empty()) return;
  Context *thisCtx = isFuncSuperContext ? parent->ctx : new Context(parent->level + 1, parentItem->children.size(), 0,
                     parent->ctx);
  thisCtx->global = parent->ctx->global;
  for (PreParserItem *item : parentItem->children) {
    CompileItem *next = new CompileItem();
    next->root = parent->root;
    next->ctx = thisCtx;
    next->level = parent->level + 1;
    next->line = item->pos;
    next->parent = parent;
    compileExpression(item, thisCtx, next);
    advance(next, item);
    parent->children.push_back(next);
  }
}

void
skx::TreeCompiler::compileCondition(std::string &content, skx::Context *ctx, skx::CompileItem *target, bool isElseIf) {
  auto spaceSplit = skx::Utils::split(content.substr(isElseIf ? 7 : 3), " ");
  target->isElseIf = isElseIf;
  Comparison *currentOperator = nullptr;
  uint8_t state = 0;
  ConditionCombineType lastCombinator = START;
  std::string last;
  uint32_t len = isElseIf ? 7 : 3;
  for (uint32_t i = 0; i < spaceSplit.size(); ++i) {
    auto current = spaceSplit[i];
    if (current[current.length() - 1] == ':') {
      current = current.substr(0, current.length() - 1);
    }
    if (current.find('"') == 0 && state == 2) {
      uint32_t x = i;
      while (spaceSplit[i][spaceSplit[i].length() - 1] != '"' && i < spaceSplit.size() - 1) {
        i++;
        if (i != x)
          current = current.append(" " + spaceSplit[i]);

      }
      if (current[current.length() - 1] == ':') current = current.substr(0, current.length() - 1);
      auto* templateResult = StringTemplateCompiler::compile(current.substr(1, current.length() - 2), ctx, target);
      if(templateResult) {
        currentOperator->target = templateResult;
      } else {
        TString *f = new TString(current.substr(1, current.length() - 2));
        currentOperator->target = new OperatorPart(LITERAL, STRING, f, false);

      }
      target->comparisons.push_back(currentOperator);
      state = 0;
      currentOperator = nullptr;

    }
    if(current == "set" && last == "is" && state == 1) {
      currentOperator->target = new OperatorPart(LITERAL, UNDEFINED, nullptr, false);
      currentOperator->type = NOT_EQUAL;
      target->comparisons.push_back(currentOperator);
      state = 0;
      currentOperator = nullptr;
    }
    if ((current == "true" || current == "false") && state == 2) {
      currentOperator->target = new OperatorPart(LITERAL, BOOLEAN, new TBoolean(current == "true"), false);

      target->comparisons.push_back(currentOperator);
      state = 0;
      currentOperator = nullptr;

    }
    if (isNumber(current[0])) {
      if (state == 2) {
        currentOperator->target = skx::Literal::extractNumber(current);
        target->comparisons.push_back(currentOperator);
        state = 0;
        currentOperator = nullptr;
      } else if (state == 0) {
        state++;
        currentOperator = new Comparison(lastCombinator);
        currentOperator->source = skx::Literal::extractNumber(current);

      }
    }
    if(current.find("arg-") == 0) {
      Variable* var = skx::Utils::searchRecursive(current, ctx);
      if(var) {
        if (state == 0) {
          state++;
          currentOperator = new Comparison(lastCombinator);
          currentOperator->source = new OperatorPart(VARIABLE, var->type, var, var->isDouble);
        } else if (state == 2) {
          currentOperator->target = new OperatorPart(VARIABLE, var->type, var, var->isDouble);
          target->comparisons.push_back(currentOperator);
          state = 0;
          currentOperator = nullptr;
        }

      }
    }
    if (isVar(current) && (state == 2 || isOperator(spaceSplit[i+1]) || spaceSplit[i+1] == "is"))  {
      auto descriptor = skx::Variable::extractNameSafe(current);
      Variable *var = skx::Utils::searchVar(descriptor, ctx);
      if (!var) {
        std::cout << "[WARNING] Condition Variable not found: " << descriptor->name << " at: " << target->line
                  << "\n";
      } else {
        if (state == 0) {
          state++;
          currentOperator = new Comparison(lastCombinator);
          if(descriptor->listAccessor == nullptr) {
            currentOperator->source = new OperatorPart(VARIABLE, var->type, var, var->isDouble);
          } else {
            if(descriptor->listAccessor->type == VAR_VALUE) {
              Variable* indexValue = skx::Utils::searchRecursive(descriptor->listAccessor->name, ctx);
              if(indexValue) {
                currentOperator->source = new OperatorPart(VARIABLE, var->type, var, indexValue, false, false);
              }
            }
          }
        } else if (state == 2) {
          if(descriptor->listAccessor == nullptr) {
            currentOperator->target = new OperatorPart(VARIABLE, var->type, var, var->isDouble);
          } else {
            if(descriptor->listAccessor->type == VAR_VALUE) {
              Variable* indexValue = skx::Utils::searchRecursive(descriptor->listAccessor->name, ctx);
              if(indexValue) {
                currentOperator->target = new OperatorPart(VARIABLE, var->type, var, indexValue, false, false);
              }
            }
          }
          target->comparisons.push_back(currentOperator);
          state = 0;
          currentOperator = nullptr;

        }
      }
      delete descriptor;
    }

    if (state == 0) {
      std::string x = content.substr(len);
      OperatorPart *part = compileExecutionValue(x, ctx, target, isElseIf);
      if (part != nullptr && currentOperator == nullptr) {
        currentOperator = new Comparison(lastCombinator);
        currentOperator->source = part;
        state++;
      }
    } else if (state == 2) {
      std::string x = content.substr(len);
      if (x[x.length() - 1] == ':') {
        x = x.substr(0, x.length() - 1);
      }
      OperatorPart *part = compileExecutionValue(x, ctx, target, isElseIf);
      if (part != nullptr) {
        if (currentOperator == nullptr) currentOperator = new Comparison(lastCombinator);
        currentOperator->target = part;
        target->comparisons.push_back(currentOperator);
        state = 0;
        currentOperator = nullptr;

      }
    }


    if (current == "is" || current == "==") {
      currentOperator->type = EQUAL;
      if (state < 2) state++;
    } else if (current == "equal") {
      if ((currentOperator->type == SMALLER || currentOperator->type == BIGGER) && last == "or") {
        if (currentOperator->type == SMALLER) currentOperator->type = SMALLER_OR_EQUAL;
        if (currentOperator->type == BIGGER) currentOperator->type = BIGGER_OR_EQUAL;
        if (state < 2) state++;

      }
    } else if (current == "greater" || current == "bigger") {
      currentOperator->type = BIGGER;
      if (state < 2) state++;
    } else if (current == "smaller") {
      currentOperator->type = SMALLER;
      if (state < 2) state++;
    } else if (current == "not") {
      currentOperator->inverted = !currentOperator->inverted;
      if (state < 2) state++;
    }
    if((current == "||" || current == "or") && state == 0)
      lastCombinator = OR;

    if((current == "&&" || current == "and") && state == 0)
      lastCombinator = AND;


    last = current;
    len += current.length() + 1;
  }
  if(state == 1) {
    currentOperator->target = new OperatorPart(LITERAL, BOOLEAN, new TBoolean(true), false);
    currentOperator->type= EQUAL;
    target->comparisons.push_back(currentOperator);
    state = 0;
    currentOperator = nullptr;
  }
  if (currentOperator != nullptr) {
    delete currentOperator;
  }
}

void skx::TreeCompiler::compileAssigment(const std::string &content, skx::Context *ctx, skx::CompileItem *target) {
  VarType targetType = UNDEFINED;
  bool hasType = content[3] == ':';
  if(hasType) {
    std::string type = content.substr(4, content.find(" ")-4);

    if(type == "string")
      targetType = STRING;

    if(type == "number")
        targetType = NUMBER;
  }
  auto spaceSplit = skx::Utils::split(content.substr(hasType ? content.find(" ") : 4), " ");
  Assigment *assigment = nullptr;
  uint8_t step = 0;
  uint32_t pos = 4;
  bool created = false;
  for (uint32_t i = 0; i < spaceSplit.size(); ++i) {
    auto current = spaceSplit[i];
    if (step == 0 && !target->assignments.empty() && isOperator(current)) {
      assigment = new Assigment();
      auto *last = target->assignments[target->assignments.size() - 1];
      Variable *lastVar = static_cast<Variable *>(last->target->value);
      if(last->target->isList)
        //                                                                                                                          Todo, need to derive free from parent?
        assigment->target = new OperatorPart(VARIABLE, lastVar->type, lastVar, last->target->indexDescriptor, last->target->isDouble, false);
      else
        assigment->target = new OperatorPart(VARIABLE, lastVar->type, lastVar, lastVar->isDouble);
      step = 2;
      assigment->type = getOperator(current);
      pos += current.length() + 1;
      continue;
    }
    if (current.find('"') == 0 && step == 2) {
      uint32_t x = i;
      if(current.length() == 1 || current[current.length()-1] != '"') {
        while ((((spaceSplit[i][spaceSplit[i].length() - 1] != '"' || spaceSplit[i][spaceSplit[i].length() - 2] == '\\' )) && i < spaceSplit.size() - 1) || i == x) {
          i++;
          if (i != x)
            current = current.append(" " + spaceSplit[i]);

        }
      }

      //      if (current == "\"\"" && i == (x + 1)) current = "\" \"";
      if (current[current.length() - 1] == ':') current = current.substr(0, current.length() - 1);
      current = skx::Utils::unescape(current);
      auto* templateResult = StringTemplateCompiler::compile(current.substr(1, current.length() - 2), ctx, target);
      if(templateResult) {
        assigment->source = templateResult;
      } else {
        TString *f = new TString(current.substr(1, current.length() - 2));
        assigment->source = new OperatorPart(LITERAL, STRING, f, false);

      }
      target->assignments.push_back(assigment);
      step = 0;
      assigment = nullptr;
    } else if ((current == "true" || current == "false") && step == 2) {
      assigment->source = new OperatorPart(LITERAL, BOOLEAN, new TBoolean(current == "true"), false);
      target->assignments.push_back(assigment);
      assigment = nullptr;
      step = 0;
      created = false;
    } else if (isNumber(current[0]) && assigment) {

      OperatorPart *num = skx::Literal::extractNumber(current);
      if (num != nullptr) {
        assigment->source = num;
      }

        static_cast<Variable*>(assigment->target->value)->type =  NUMBER;
      target->assignments.push_back(assigment);
      step = 0;
      assigment = nullptr;
    } else if (current.find("arg-") == 0) {

      Variable* var = skx::Utils::searchRecursive(current, ctx);
      if(var) {
        assigment->source = new OperatorPart(VARIABLE, var->type, var, var->isDouble);
        target->assignments.push_back(assigment);
        step = 0;
        assigment = nullptr;
      }
    }
    if (step == 2) {
      auto funcCallMatches = skx::RegexUtils::getMatches(skx::functionCallPattern, content);
      if (!(funcCallMatches).empty()) {
        auto entry = funcCallMatches[0];
        std::string base = entry.content;
        std::string name = base.substr(0, base.find_first_of(" \n\r\t\f\v("));
        size_t paramsStart = base.find_first_of('(');
        size_t paramsEnd = base.find_last_of(')');
        std::string params = base.substr(paramsStart + 1, paramsEnd - paramsStart - 1);
        auto *call = new FunctionInvoker();
        call->function = ctx->global->functions[name];

        if (skx::Utils::trim(params).length() > 0) {
          for (auto const &param : skx::Utils::split(params, ",")) {
            auto trimmed = skx::Utils::trim(param);
            auto descriptor = skx::Variable::extractNameSafe(trimmed);
            Variable *var = skx::Utils::searchVar(descriptor, ctx);
            if (var)
              call->dependencies.push_back(new OperatorPart(VARIABLE, var->type, var, var->isDouble));
            else
              std::cout << "[WARNING] Assigment Variable for function not found: " << descriptor->name << " at: "
                        << target->line << "\n";
            delete descriptor;
          }
        }
        assigment->source = new OperatorPart(EXECUTION, UNDEFINED, call, false);
        target->assignments.push_back(assigment);
        assigment = nullptr;
        step = 0;
        created = false;
      } else {
        std::string x = content.substr(pos);
        OperatorPart* targetOperator = compileExecutionValue(x, ctx, target);
        if(targetOperator != nullptr) {
          assigment->source = targetOperator;
          target->assignments.push_back(assigment);
          assigment = nullptr;
          step = 0;
          created = false;
        }
      }
    }

    if (isVar(current)) {
      auto descriptor = skx::Variable::extractNameSafe(current);
      Variable *currentVar = nullptr;

      if (descriptor->type == STATIC || descriptor->type == GLOBAL) {
        currentVar = skx::Utils::searchRecursive(descriptor->name, ctx->global);
      } else {
        currentVar = skx::Utils::searchRecursive(descriptor->name, ctx);
      }

      if (currentVar == nullptr) {
        created = true;
        Context *targetCtx = descriptor->type == CONTEXT ? ctx : ctx->global;
        created = true;
        currentVar = new Variable();
        currentVar->name = descriptor->name;
        currentVar->accessType = descriptor->type;
        currentVar->setValue(nullptr);
        currentVar->ctx = targetCtx;
        currentVar->type = targetType;
        //   currentVar->created = created;
        targetCtx->vars[descriptor->name] = currentVar;
      }
      if (assigment == nullptr || assigment->target == nullptr) {
        assigment = new Assigment();
        if(descriptor->listAccessor == nullptr) {
          assigment->target = new OperatorPart(VARIABLE, currentVar->type, currentVar, currentVar->isDouble);

        } else {
          Variable* indexValue = skx::Utils::searchRecursive(descriptor->listAccessor->name, ctx);
          if(indexValue) {
            assigment->target = new OperatorPart(VARIABLE, currentVar->type, currentVar, indexValue, false, false);
          }
        }
        step++;
        delete descriptor;
      } else {
        delete descriptor;
        if (step == 2) {
          assigment->source = new OperatorPart(VARIABLE, currentVar->type, currentVar, currentVar->isDouble);
          if (created) {
            assigment->target->type = currentVar->type;
          }
          target->assignments.push_back(assigment);
          assigment = nullptr;
          step = 0;
          created = false;
        }//  else {
        //     if (currentVar != nullptr) {
        //         currentVar->ctx->vars.erase(currentVar->name);
        //         delete currentVar;
        //     }
        // }
      }
    }
    if (assigment != nullptr) {
      if (current == "to") {
        assigment->type = ASSIGN;
        if (step < 2) step++;
      }
    }
    pos += current.length() + 1;
  }
  int count = 1;
  for (uint32_t i = 0; i < target->assignments.size(); ++i) {
    if(i == 0) continue;
    if(target->assignments[i]->type == MULTIPLY || target->assignments[i]->type == DIVIDE) {
      if(target->assignments[count]->type == ADD || target->assignments[count]->type == SUBTRACT) {
        auto from = target->assignments[count];
        target->assignments[count] = target->assignments[i];
        target->assignments[i] = from;
        count++;
      }
    }
  }
}

void skx::TreeCompiler::compileOperator(std::string &content, skx::Context *pContext, skx::CompileItem *pItem) {
  auto spaceSplit = skx::Utils::split(content, " ");
  auto *assigment = new Assigment();
  if (spaceSplit[0] == "add") assigment->type = ADD;
  if (spaceSplit[0] == "subtract") assigment->type = SUBTRACT;
  if (spaceSplit[0] == "multiply") assigment->type = MULTIPLY;
  if (spaceSplit[0] == "divide") assigment->type = DIVIDE;

  OperatorPart *source = skx::Literal::extractNumber(spaceSplit[1]);
  auto varName = spaceSplit[2];
  auto *var = skx::Utils::searchVar(skx::Variable::extractNameSafe(spaceSplit[3]), pContext);
  if (var == nullptr) return;
  assigment->source = source;
  assigment->target = new OperatorPart(VARIABLE, var->type, var, var->isDouble);
  pItem->assignments.push_back(assigment);
}

void skx::TreeCompiler::setupFunctionMeta(std::string &content, skx::Function *target) {
  auto base = content.substr(8);
  std::string name = skx::Utils::ltrim(base);
  name = name.substr(0, name.find_first_of(" \n\r\t\f\v("));
  size_t paramsStart = base.find_first_of('(');
  size_t paramsEnd = base.find_last_of(')');
  if (base.find("::") != std::string::npos) {
    auto returnType = base.substr(base.find("::"));
    returnType = skx::Utils::trim(returnType.substr(2, returnType.length() - 3));
    if (returnType == "text" || returnType == "string") {
      target->returnType = STRING;
    } else if (returnType == "number" || returnType == "integer" || returnType == "int") {
      target->returnType = NUMBER;
    } else if (returnType == "bool" || returnType == "boolean") {
      target->returnType = BOOLEAN;
    }
  }
  std::string params = base.substr(paramsStart + 1, paramsEnd - paramsStart - 1);
  if (!params.empty()) {
    for (auto const &param : skx::Utils::split(params, ",")) {
      auto trimmed = skx::Utils::trim(param);
      VariableDescriptor *descriptor = new VariableDescriptor();
      if (trimmed.find(':') == std::string::npos) {
        descriptor->type = CONTEXT;
        descriptor->name = trimmed;
      } else {
        auto typeSplit = skx::Utils::split(trimmed, ":");
        auto actualName = skx::Utils::trim(typeSplit[0]);
        auto type = skx::Utils::trim(typeSplit[1]);
         descriptor->varType = type;
        //TODO implement actual param type & default value, also what if the default value has a ,?
        //auto actualType = skx::Utils::trim(typeSplit[1]);
        descriptor->type = CONTEXT;
        descriptor->name = actualName;
      }
      target->targetParams.push_back(descriptor);
    }
  }
  target->name = name;
}

void skx::TreeCompiler::compileExecution(std::string &content, skx::Context *context, skx::CompileItem *target) {
  if (content.find("to console") != std::string::npos) {
    auto *pr = new Print();
    auto spaceSplit = skx::Utils::split(content, " ");
    size_t pos = 0;
    for (uint32_t i = 0; i < spaceSplit.size(); i++) {
      auto current = spaceSplit[i];
      if (current.find('"') == 0) {
        uint32_t x = i;
        while (spaceSplit[i][spaceSplit[i].length() - 1] != '"' && i < spaceSplit.size() - 1) {
          i++;
          if (i != x)
            current = current.append(" " + spaceSplit[i]);

        }
        if (current[current.length() - 1] == ':') current = current.substr(0, current.length() - 1);
        auto* templateResult = StringTemplateCompiler::compile(current.substr(1, current.length() - 2), context, target);
        if(templateResult) {
          pr->dependencies.push_back(templateResult);
        } else {
        TString *f = new TString(current.substr(1, current.length() - 2));
        pr->dependencies.push_back(new OperatorPart(LITERAL, STRING, f, false));

        }

      }
      if (isVar(current)) {
        auto descriptor = skx::Variable::extractNameSafe(current);
        Variable *var = skx::Utils::searchVar(descriptor, context);
        if (var)
          if(descriptor->listAccessor) {
            Variable* indexValue = skx::Utils::searchRecursive(descriptor->listAccessor->name, context);
            if(indexValue) {
              pr->dependencies.push_back( new OperatorPart(VARIABLE, var->type, var, indexValue, false, false));
            }
          } else {
            pr->dependencies.push_back(new OperatorPart(VARIABLE, var->type, var, var->isDouble));
          } else
          std::cout << "[WARNING] Exec Variable not found: " << descriptor->name << " at: " << target->line
                    << "\n";
        delete descriptor;
      }
      if(isNumber(current[0])) {
        pr->dependencies.push_back(skx::Literal::extractNumber(current));
      }
      pos += current.length();
    }
    if (pr->dependencies.size() > 0)
      target->executions.push_back(pr);
    else
      delete pr;
    return;
  } else if(content.find("nativecall") == 0) {
    auto* exec = skx::NativeCallCompiler::compileCall(content, context, target);
  if(exec)
    target->executions.push_back(exec);
  }else {
    auto funcCallMatches = skx::RegexUtils::getMatches(skx::functionCallPattern, content);
    if (!(funcCallMatches).empty()) {
      auto entry = funcCallMatches[0];
      std::string base = entry.content;
      std::string name = base.substr(0, base.find_first_of(" \n\r\t\f\v("));
      size_t paramsStart = base.find_first_of('(');
      size_t paramsEnd = base.find_last_of(')');
      std::string params = base.substr(paramsStart + 1, paramsEnd - paramsStart - 1);
      auto *call = new FunctionInvoker();
      call->function = context->global->functions[name];

      if (params.length() > 0)
        for (auto const &param : skx::Utils::split(params, ",")) {
          auto trimmed = skx::Utils::trim(param);
          if(isNumber(trimmed[0])) {
            call->dependencies.push_back(skx::Literal::extractNumber(trimmed));
          } else if (trimmed[0] == '"') {
            call->dependencies.push_back(new OperatorPart(LITERAL, STRING, new TString(trimmed.substr(1, trimmed.length() -1)), false));
          } else {
            auto descriptor = skx::Variable::extractNameSafe(trimmed);
            Variable *var = nullptr;
            if (descriptor->type == STATIC || descriptor->type == GLOBAL) {
              var = skx::Utils::searchRecursive(descriptor->name, context->global);
            } else {
              var = skx::Utils::searchRecursive(descriptor->name, context);
            }
            if (!var) {
              std::cout << "[WARNING] Call Param not found: " << descriptor->name << " at: " << target->line
                        << "\n";
              delete descriptor;
              continue;
            }
            delete descriptor;
            call->dependencies.push_back(new OperatorPart(VARIABLE, var->type, var, var->isDouble));
          }
        }
      target->executions.push_back(call);
    } else {
      if (content.find("json") != std::string::npos) {
        Json::compileRequest(content, context, target);
        return;
      }
#ifdef SKX_BUILD_API
      CompileItem *t = target->root;
      if(t->triggers.size() == 1 && t->triggers[0]->type == MC_EVENT) {
        if(content.find("player") != std::string::npos) {
          Execution* out = PlayerAction::compilePlayerInstruction(content, context, target);
          if(out != nullptr) target->executions.push_back(out);
        } else {
          skx::EventBaseEffectCompiler::compile(content, context, target);
        }
      }
#endif
    }
  }
}

void skx::TreeCompiler::compileReturn(std::string &basicString, skx::Context *pContext, skx::CompileItem *pItem) {
  auto spaceSplit = skx::Utils::split(basicString.substr(7), " ");

  for (uint32_t i = 0; i < spaceSplit.size(); i++) {
    auto current = spaceSplit[i];
    if (current.find('"') == 0) {
      uint32_t x = i;
      while (spaceSplit[i][spaceSplit[i].length() - 1] != '"' && i < spaceSplit.size() - 1) {
        i++;
        if (i != x)
          current = current.append(" " + spaceSplit[i]);

      }
      if (current[current.length() - 1] == ':') current = current.substr(0, current.length() - 1);
      TString *f = new TString(current.substr(1, current.length() - 2));
      pItem->returner = new ReturnOperation();
      pItem->returner->targetReturnItem = new OperatorPart(LITERAL, STRING, f, false);

    } else if (current == "true" || current == "false") {
      TBoolean *f = new TBoolean(current == "true");
      pItem->returner = new ReturnOperation();
      pItem->returner->targetReturnItem = new OperatorPart(LITERAL, BOOLEAN, f, false);

    } else if (isNumber(current[0])) {
      pItem->returner = new ReturnOperation();
      pItem->returner->targetReturnItem = skx::Literal::extractNumber(current);

    } else if (isVar(current)) {
      auto descriptor = skx::Variable::extractNameSafe(current);
      Variable *var = skx::Utils::searchVar(descriptor, pContext);
      if (var) {
        pItem->returner = new ReturnOperation();
        pItem->returner->targetReturnItem = new OperatorPart(VARIABLE, var->type, var, var->isDouble);
      } else {
        std::cout << "[WARNING] Variable in return not found: " << descriptor->name << " at: " << pItem->line
                  << "\n";
      }
      delete descriptor;
    }

  }
}

void skx::TreeCompiler::compileLoop(const std::string &content, skx::Context *ctx, skx::CompileItem *target) {

  std::cout << content.substr(5, content.length() - 6) << "\n";
  Loop *loop = new Loop();
  loop->fromFunction = isFunction;
  loop->rootItem = target;
  target->isLoop = true;
  auto spaceSplit = skx::Utils::split(content.substr(5, content.length() - 6), " ");
  if(isVar(spaceSplit[0])) {
    VariableDescriptor* descriptor = skx::Variable::extractNameSafe(spaceSplit[0]);
    if(descriptor && descriptor->listAccessor != nullptr) {
      loop->isIterator = true;
      if(descriptor->listAccessor && descriptor->listAccessor->type == ALL) {
        Variable *var = skx::Utils::searchVar(descriptor, ctx);
        if(var) {
          loop->iteratorVar = var;
          std::string loopIndexName = spaceSplit.size() == 4 && spaceSplit[2] == "as" ? spaceSplit[3] : "loop-value";
          Variable *loopValue = new Variable();
          loopValue->name = loopIndexName;
          loopValue->isDouble = false;
          loopValue->accessType = CONTEXT;
          loopValue->contextValue = true;
          loopValue->setValue(nullptr);
          loop->iteratorValue = loopValue;
          ctx->vars[loopIndexName] = loopValue;
          Variable *loopIndex = new Variable();
          loopIndex->name = "loop-index";
          loopIndex->type = STRING;
          loopIndex->isDouble = false;
          loopIndex->accessType = CONTEXT;
          loopIndex->contextValue = true;
          loopIndex->setValue(new TString());
          loop->loopCounter = loopIndex;
          ctx->vars["loop-index"] = loopIndex;
          target->executions.push_back(loop);
          return;
        }
      }
    }
    if(descriptor != nullptr)
      delete descriptor;
  }
  if (spaceSplit[0] == "while") {
    loop->hasCondition = true;
    loop->comparison = new Comparison();
    if (spaceSplit.size() > 1 && (spaceSplit[1] == "true" || spaceSplit[1] == "false")) {
      bool v = spaceSplit[1] == "true";
      loop->comparison->type = EQUAL;
      loop->comparison->target = new OperatorPart(LITERAL, BOOLEAN, new TBoolean(true), false);
      loop->comparison->source = new OperatorPart(LITERAL, BOOLEAN, new TBoolean(v), false);
      target->executions.push_back(loop);
      return;
    }
  } else if (spaceSplit[1] == "times") {
    if (isVar(spaceSplit[0])) {
      VariableDescriptor *descriptor = skx::Variable::extractNameSafe(spaceSplit[0]);
      Variable *var = skx::Utils::searchVar(descriptor, ctx);
      if (var) {
        loop->loopTargetVar = var;
      } else {
        std::cout << "[WARNING] Variable for loop target not found: " << descriptor->name << " at: "
                  << target->line
                  << "\n";
        delete descriptor;
        delete loop;
        return;
      }
      delete descriptor;
    } else {
      loop->loopTarget = std::stoi(spaceSplit[0]);
    }
    std::string loopIndexName = spaceSplit.size() == 4 && spaceSplit[2] == "as" ? spaceSplit[3] : "loop-value";
    Variable *loopValue = new Variable();
    loopValue->name = loopIndexName;
    loopValue->type = NUMBER;
    loopValue->isDouble = false;
    loopValue->accessType = CONTEXT;
    loopValue->contextValue = true;
    loopValue->setValue(new TNumber(0));
    ctx->vars[loopIndexName] = loopValue;
    loop->loopCounter = loopValue;
    target->executions.push_back(loop);
  } else {
    std::cout << "[WARNING] Invalid loop: " << content << " at: " << target->line
              << "\n";
    delete loop;
    target->isLoop = false;
  }

}

bool skx::TreeCompiler::isVar(std::string &val) {
  return (val[0] == '{' && val[val.length() - 1] == '}') || val == "player";
}

bool skx::TreeCompiler::isNumber(char c) {
  return c >= '0' && c <= '9';
}

bool skx::TreeCompiler::isOperator(std::string &in) {
  return in == "+" || in == "-" || in == "*" || in == "/"
         || in == "plus" || in == "minus" || in == "multiply" || in == "divided";
}

skx::InstructionOperator skx::TreeCompiler::getOperator(std::string &in) {
  if (in == "+" || in == "plus") return ADD;
  if (in == "-" || in == "minus") return SUBTRACT;
  if (in == "*" || in == "multiply") return MULTIPLY;
  if (in == "/" || in == "divided") return DIVIDE;
  return NOT_EQUAL;
}

void skx::TreeCompiler::compileTrigger(std::string &content, skx::Context *context, skx::CompileItem *target) {
  if (content == "on load:" || content == "on unload:") {
    TriggerSignal *signal = new TriggerSignal();
    signal->signalType = content == "on unload:" ? TriggerSignal::UN_LOAD : TriggerSignal::LOAD;
    target->triggers.push_back(signal);
    return;
  }
#ifdef SKX_BUILD_API
  if (content.find("command") == 0) {
    TriggerCommand* cmd = new TriggerCommand();
    auto split = skx::Utils::split(content, " ");
    for (uint32_t i = 0; i < split.size(); ++i) {
      if(i == 0) continue;
      if(i == 1) {
        cmd->name = split[i];
      } else {
        int index = i-1;
        std::string name = "arg-" + std::to_string(index);
        Variable* var = new Variable();
        var->name = name;
        var->type = STRING;
        var->accessType = CONTEXT;
        var->ctx = context;
        context->vars[name] = var;
        cmd->args.push_back(var);
      }
    }
    Variable* player = new Variable();
    player->name = "player";
    player->accessType = CONTEXT;
    player->ctx = context;
    player->type = POINTER;
    player->customTypeName = "std::mc::player";
    player->setValue(new TPlayerRef(cmd));
    context->vars["player"] = player;
    target->triggers.push_back(cmd);
  } else {
    std::string type = skx::Utils::getEventClassFromExpression(content.substr(0, content.length() - 1));
    if (type.length() == 0) {
      std::cout << "Unknown event: " << content << " at: " << target->line << "\n";
      return;
    }
    TriggerEvent* evInstance = new TriggerEvent(type);
    Variable* player = new Variable();
    player->name = "player";
    player->accessType = CONTEXT;
    player->ctx = context;
    player->type = POINTER;
    player->customTypeName = "std::mc::player";
    player->setValue(new TPlayerRef(evInstance));
    context->vars["player"] = player;
    EventPreconditions::compilePreConditions(content, evInstance, context);
    target->triggers.push_back(evInstance);
  }
#endif
}

skx::OperatorPart *
skx::TreeCompiler::compileExecutionValue(std::string &content, skx::Context *ctx, skx::CompileItem *target,
    bool isElseIf) {

  if (content.find("json") != std::string::npos) {
    return skx::Json::compileCondition(content, ctx, target);
  } else if (content.find("nativecall") == 0) {
    auto* exec = skx::NativeCallCompiler::compileCall(content, ctx, target);
    return new OperatorPart(EXECUTION, UNDEFINED, exec, false);
  }
  if(content.find('{') == 0 && content.find(" contains ") != std::string::npos) {
    auto split = skx::Utils::split(content[content.length() -1] == ':' ? content.substr(0, content.length() -1) : content, " ");
    MapEffects* effect = new MapEffects();
    effect->execType = MapEffects::CONTAINS;

    VariableDescriptor* mapDesc = skx::Variable::extractNameSafe(split[0]);
    if(!mapDesc) return nullptr;
    Variable* map = skx::Utils::searchVar(mapDesc, ctx);
    if(!map) return nullptr;
    effect->dependencies.push_back(new OperatorPart(VARIABLE, MAP, map, false));
    auto val = split[2];
    if(isVar(val)) {
      VariableDescriptor* indexDesc = skx::Variable::extractNameSafe(val);
      if(indexDesc) {
        Variable* ind = skx::Utils::searchVar(indexDesc, ctx);
        effect->dependencies.push_back(new OperatorPart(VARIABLE, ind->type, ind, false));

      }
      delete indexDesc;
    } else if(val[0] == '"') {
      if (val[val.length() - 1] == '"') val = val.substr(0, val.length() - 1);

      effect->dependencies.push_back(new OperatorPart(LITERAL, STRING, new TString(val.substr(1, val.length() -1)), false));
    }
    delete mapDesc;
    if(effect->dependencies.size() == 2) {
      return new OperatorPart(EXECUTION, BOOLEAN, effect, false);
    } else {
      delete effect;
    }
  }

#ifdef SKX_BUILD_API
  CompileItem *t = target->root;
  if(t->triggers.size() == 1 && t->triggers[0]->type == MC_EVENT) {
    return skx::EventValuesCompiler::compile(content, ctx, target, isElseIf);
  }
#endif
  return nullptr;
}

skx::CompileItem::~CompileItem() {
  for (auto &i : children) {
    delete i;
  }
  children.clear();
  for (auto &assignment : assignments) {
    delete assignment;
  }
  assignments.clear();
  for (auto &execution : executions) {
    delete execution;
  }
  executions.clear();
  for (auto &comparison : comparisons) {
    delete comparison;
  }
  for (auto &trigger : triggers) {
    delete trigger;
  }
  triggers.clear();
  if (returner) {
    delete returner->targetReturnItem;
    delete returner;
  }
  comparisons.clear();
}
