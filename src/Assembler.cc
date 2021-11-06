//
// Created by liz3 on 16/07/2020.
//

#include "../include/Assembler.h"
#include "../include/BasicInstructions.h"
std::string skx::Assembler::assemble(skx::Script* script) {
  skx::Assembler assembler;
  for(auto& entry : script->baseContext->functions) {
    assembler.assembleFunction(entry.second, nullptr);
  }
  AssemblerContext ctx(nullptr);
  for(auto& entry : script->signals) {
    assembler.assembleTreeWithName(entry.second, &ctx, "_main");
  }
  std::string sum;
  sum.append(".text\n");
  sum.append(".global _main\n\n");
  sum.append(".align 2\n");
  for(auto& entry : assembler.sections) {
    sum.append(entry.second->serialise() + "\n");
  }
  sum.append("\n");
  sum.append(".data\n\n");
  sum.append(assembler.data.serialise());
  return sum;
}

void skx::Assembler::assembleFunction(skx::Function* function, AssemblerContext* parentContext) {
  AssemblerContext context(parentContext);
  AssemblySection* section = getSection("func_" + function->name);
  reportVars(function->functionItem, &context);
  section->add(context.getSize(false));
  for(size_t i = 0; i < function->targetParams.size(); i++) {
    std::string index = getNameForFunctionParameter(i);
    std::string targetName = context.add(function->functionItem->ctx->vars[function->targetParams[i]->name]);
    section->add("stur " + index + ", " + targetName);

  }

  for(auto& entry : context.getStackAdds(false)) {
    section->add(entry);
  }
  assembleTree(function->functionItem, &context, section, true);
  for(auto& entry : context.getStackAdds(true)) {
    section->add(entry);
  }
  section->add(context.getSize(true));
  section->add("ret");
}

void skx::Assembler::assembleTree(CompileItem* item, AssemblerContext* context, AssemblySection* section, bool omitStackSize) {
  reportVars(item, context);
  if(!omitStackSize)
    section->add(context->getSize(false));
  for(CompileItem* child : item->children)
    walk(child, context, section);
  if(!omitStackSize)
    section->add(context->getSize(true));
}
void skx::Assembler::reportVars(CompileItem* ctx, AssemblerContext* asCtx) {
  for(auto& entry : ctx->ctx->vars) {
    asCtx->add(entry.second);
  }
  for(auto& item : ctx->children)
    reportVars(item, asCtx);
}
void skx::Assembler::walk(CompileItem* item, AssemblerContext* context, AssemblySection* section) {
  if(item->returner) {
    if(item->returner->targetReturnItem != nullptr) {
      switch(item->returner->targetReturnItem->operatorType) {
        case VARIABLE: {
          Variable* var = static_cast<Variable*>(item->returner->targetReturnItem->value);
          std::string name = context->add(var);
          section->add("ldur x0, " + name);
         break;
        }
        default: {
          break;
        }
      }
    } else {
      section->add("ret");
    }
    return;
  }
  if(item->comparisons.size() > 0) {
    auto* comp = item->comparisons[0];
    AssemblySection * childSection = getSection("");
    std::string left;
    bool leftIsVar = false;
    std::string right;
    bool rightIsVar = false;
    std::string operation;
    switch(comp->source->operatorType) {
    case LITERAL: {
      TNumber* number = static_cast<TNumber*>(comp->source->value);
      left = "#" + std::to_string(number->intValue);
      break;
    }
    case VARIABLE: {
      leftIsVar = true;
      left = context->add(static_cast<Variable*>(comp->source->value));
      break;
    }
    default: {
      break;
    }
    }
    switch(comp->target->operatorType) {
    case LITERAL: {
      TNumber* number = static_cast<TNumber*>(comp->target->value);
      right = "#" + std::to_string(number->intValue);
      break;
    }
    case VARIABLE: {
      rightIsVar = true;
      right = context->add(static_cast<Variable*>(comp->target->value));
      break;
    }
    default: {
      break;
    }
    }
    section->add(std::string(leftIsVar ? "ldur" : "mov") + " x10, " + left);
    section->add(std::string(rightIsVar ? "ldur" : "mov") + " x11, " + right);
    section->add("cmp x10, x11");
    section->add("b.ne " + childSection->name);
    assembleTree(item, context, section, true);
    section->add(childSection->name + ":");

  } else if(item->executions.size() > 0) {
    for(auto& execution : item->executions) {
      if(Print* printer = dynamic_cast<Print*>(execution)) {
        if(printer->dependencies[0]->operatorType == VARIABLE) {

        Variable* target = static_cast<Variable*>(printer->dependencies[0]->value);
        std::string name = context->add(target);
        std::string first = getNameForFunctionParameter(0);
        std::string second = getNameForFunctionParameter(1);
        std::string format;
        if(target->type == STRING) {
         format = data.getOrRegisterStatic("pf_str", "%s\\n");
         } else if (target->type == NUMBER) {
          format = data.getOrRegisterStatic("pf_num", "%ld\\n");
        }
        section->add("adrp " + first + ", " + format + "@PAGE");
        section->add("add " + first + ", " + first + ", " + format + "@PAGEOFF");
        section->add("ldur " + second + ", " + name);
        section->add("sub sp, sp, #16");
        section->add("stur " + second + ", [sp, #0]");
        section->add("bl _printf");
        section->add("add sp, sp, #16");
        } else if (printer->dependencies[0]->operatorType == LITERAL) {

        VariableValue* target = static_cast<VariableValue*>(printer->dependencies[0]->value);
        std::string name = data.getOrRegister(target);
        std::string first = getNameForFunctionParameter(0);
        std::string second = getNameForFunctionParameter(1);
        std::string format;
        if(target->type == STRING) {
         format = data.getOrRegisterStatic("pf_str", "%s\\n");
         } else if (target->type == NUMBER) {
          format = data.getOrRegisterStatic("pf_num", "%ld\\n");
        }
        section->add("adrp " + first + ", " + format + "@PAGE");
        section->add("add " + first + ", " + first + ", " + format + "@PAGEOFF");
        section->add("adrp " + second + ", " + name + "@PAGE");
        section->add("add " + second + ", " + second + ", " + name + "@PAGEOFF");
        section->add("sub sp, sp, #16");
        section->add("stur " + second + ", [sp, #0]");
        section->add("bl _printf");
        section->add("add sp, sp, #16");

        }
      } else if(FunctionInvoker* invoker = dynamic_cast<FunctionInvoker*>(execution)) {
        for(size_t i = 0; i < invoker->dependencies.size(); i++) {
          Variable* v = static_cast<Variable*>(invoker->dependencies[i]->value);
          std::string index = getNameForFunctionParameter(i);
          std::string name = context->add(v);
          section->add("ldur " + index + ", " + name);
        }

        section->add("bl func_" + invoker->function->name);

      }
    }
  }else if(item->assignments.size() > 0) {
    for(auto& assignment : item->assignments) {
      std::string target = context->add(static_cast<Variable*>(assignment->target->value));
      switch(assignment->source->operatorType) {
        case VARIABLE: {
          Variable* var = static_cast<Variable*>(assignment->source->value);
          std::string op = getOpFromType(assignment->type);
          std::string name = context->add(var);
          if (assignment->type == ASSIGN) {
            section->add("ldur x10, " + name);
            section->add("stur x10, " + target);
          } else {
          section->add("ldur x11, " + name);
          section->add("ldur x10, " + target);
          section->add(op + " x10, x10, x11");
          section->add("stur x10, " + target);
          }
          break;
        }
        case LITERAL: {
          switch(assignment->source->type) {
            case STRING: {
              std::string str_name = data.getOrRegister(static_cast<VariableValue*>(assignment->source->value));
              section->add("adrp x10, " + str_name + "@PAGE");
              section->add("add x10, x10, " + str_name + "@PAGEOFF");
              section->add("stur x10, " + target);
              break;
            }
              case NUMBER: {
                TNumber* number = static_cast<TNumber*>(assignment->source->value);
                std::string op = getOpFromType(assignment->type);

                if(assignment->type == ASSIGN) {
                  section->add("mov x10, #" + std::to_string(number->intValue));
                  }else {
                  section->add("mov x11, #" + std::to_string(number->intValue));
                  section->add("ldur x10, " + target);
                  section->add(op + " x10, x10, x11");
                }
                section->add("stur x10, " + target);

                break;
              }
            default: {
              break;
            }
          }
          break;
        }
          case EXECUTION: {
            Execution* execution = static_cast<Execution*>(assignment->source->value);
            if(FunctionInvoker* invoker = dynamic_cast<FunctionInvoker*>(execution)) {
              for(size_t i = 0; i < invoker->dependencies.size(); i++) {
                Variable* v = static_cast<Variable*>(invoker->dependencies[i]->value);
                std::string index = getNameForFunctionParameter(i);
                std::string name = context->add(v);
                section->add("ldur " + index + ", " + name);
              }

              section->add("bl func_" + invoker->function->name);
              if(assignment->type == ASSIGN) {
                section->add("stur x0, " + target);
              } else {
              std::string op = getOpFromType(assignment->type);
              section->add("ldur x10, " + target);
              section->add(op + " x10, x10, x0");
              section->add("stur x10, " + target);
              }
            }
          }
        default: {
          break;
        }
      }
    }
  }
}
std::string skx::Assembler::getNameForFunctionParameter(size_t index) {
  switch(index) {
    case 0:
      return "x0";
  case 1:
    return "x1";
    case 2:
      return "x2";
  case 3:
    return "x3";
    case 4:
      return "x4";
  case 5:
    return "x5";
   default: {
     return "";
   }
  }
}
std::string skx::Assembler::getOpFromType(InstructionOperator type) {
  switch(type) {
    case ASSIGN:
      return "mov";
  case ADD:
    return "add";
  case SUBTRACT: {
    return "sub";
  }
    default: {
      return "";
    }
  }
}
void skx::Assembler::assembleTreeWithName(CompileItem* item,AssemblerContext* context, std::string name) {
  auto* section =  getSection(name);
  assembleTree(item, context,section);
  if(name == "_main") {
    section->add("mov x0, #0");
    section->add("mov x16, #1");
    section->add("svc #0x80");
    section->add("ret");

  }
}

skx::AssemblySection* skx::Assembler::getSection(std::string name) {
  if(name.length() == 0) {
    size_t entry = sectionCount++;
    std::string sectionName = "cc_" + std::to_string(entry);
    skx::AssemblySection* section = new skx::AssemblySection(sectionName);
    return section;
  }
  if(sections.count(name))
    return nullptr;
  skx::AssemblySection* section = new skx::AssemblySection(name);
    sections[name] = section;
    return section;
}
