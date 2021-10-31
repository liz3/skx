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
  sum.append("extern _printf\n");
  sum.append("global _main\n\n");
  sum.append("section .text\n");
  for(auto& entry : assembler.sections) {
    sum.append(entry.second->serialise() + "\n");
  }
  sum.append("\n");
  sum.append("section .data\n\n");
  sum.append(assembler.data.serialise());
  return sum;
}

void skx::Assembler::assembleFunction(skx::Function* function, AssemblerContext* parentContext) {
  AssemblerContext context(parentContext);
  AssemblySection* section = getSection("func_" + function->name);
  for(size_t i = 0; i < function->targetParams.size(); i++) {
    std::string index = getNameForFunctionParameter(i);
    context.add(function->functionItem->ctx->vars[function->targetParams[i]->name], index);
  }
  for(auto& entry : context.getStackAdds(false)) {
    section->add(entry);
  }
  assembleTree(function->functionItem, &context, section);
  for(auto& entry : context.getStackAdds(true)) {
    section->add(entry);
  }
  section->add("ret");
}

void skx::Assembler::assembleTree(CompileItem* item, AssemblerContext* context, AssemblySection* section) {
  reportVars(item, context);
  section->add(context->getSize(false));
  for(CompileItem* child : item->children)
    walk(child, context, section);
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
          section->add("mov rax, " + name);
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
  if(item->executions.size() > 0) {
    for(auto& execution : item->executions) {
      if(Print* printer = dynamic_cast<Print*>(execution)) {
        if(printer->dependencies[0]->operatorType == VARIABLE) {

        Variable* target = static_cast<Variable*>(printer->dependencies[0]->value);
        std::string name = context->add(target);
        std::string first = getNameForFunctionParameter(0);
        std::string second = getNameForFunctionParameter(1);
        std::string format;
        if(target->type == STRING) {
         format = data.getOrRegisterStatic("pf_str", "\"%s\", 10");
         } else if (target->type == NUMBER) {
          format = data.getOrRegisterStatic("pf_num", "\"%ld\", 10");
        }
        section->add("mov " + first + ", " + format);
        section->add("mov " + second + ", " + name);
        section->add("push " + first);
        section->add("push " + second);

        if(context->vars.size() % 2 == 0)
          section->add("sub rsp, 8");
        section->add("call _printf");
        if(context->vars.size() % 2 == 0)
          section->add("add rsp, 8");
        section->add("pop " + second);
        section->add("pop " + first);
        } else if (printer->dependencies[0]->operatorType == LITERAL) {

        }
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
          section->add("mov rcx, " + name);
          section->add(op + " " + target + ", rcx");
          break;
        }
        case LITERAL: {
          switch(assignment->source->type) {
            case STRING: {
              std::string str_name = data.getOrRegister(static_cast<VariableValue*>(assignment->source->value));
              section->add("mov rcx, " + str_name);
              section->add("mov " + target + ", rcx");
              break;
            }
              case NUMBER: {
                TNumber* number = static_cast<TNumber*>(assignment->source->value);
                std::string op = getOpFromType(assignment->type);
                section->add("mov rcx, " + std::to_string(number->intValue));
                section->add(op + " " + target + ", rcx");
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
                section->add("mov " + index + ", " + name);
              }
              std::string op = getOpFromType(assignment->type);
              section->add("call func_" + invoker->function->name);
              section->add(op + " " + target + ", rax");
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
      return "rdi";
  case 1:
    return "rsi";
    case 2:
      return "rdx";
  case 3:
    return "rcx";
    case 4:
      return "r8";
  case 5:
    return "r9";
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
  assembleTree(item, context, getSection(name));
}

skx::AssemblySection* skx::Assembler::getSection(std::string name) {
  if(name.length() == 0) {
    size_t entry = sectionCount++;
    std::string sectionName = "cc_" + std::to_string(entry);
    skx::AssemblySection* section = new skx::AssemblySection(sectionName);
    sections[sectionName] = section;
    return section;
  }
  if(sections.count(name))
    return nullptr;
  skx::AssemblySection* section = new skx::AssemblySection(name);
    sections[name] = section;
    return section;
}
