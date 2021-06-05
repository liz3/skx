#include "../../include/api/NativeCall.h"
#include "../../include/utils.h"
#include "../../include/Literal.h"
#include "../../include/types/TString.h"
#include "../../include/types/TNumber.h"
#include "../../include/types/TBoolean.h"

#include <iostream>
#include <fstream>

skx::NativeCallInterface::CallType skx::NativeCallCompiler::getCallType(std::string &entry) {
  if(entry == "strlen") return NativeCallInterface::STRLEN;
  if(entry == "readfile") return NativeCallInterface::READFILE;
  if(entry == "writefile") return NativeCallInterface::WRITEFILE;
  return NativeCallInterface::UNKNOWN;
}

skx::OperatorPart *skx::NativeCallInterface::execute(skx::Context *target) {
  switch(type) {
    case NativeCallInterface::STRLEN: {
      auto* part = dependencies[0];
      TString* value = nullptr;
      if(part->operatorType == LITERAL) {
        value = static_cast<TString* >(part->value);
      } else if (part->operatorType == VARIABLE) {
        Variable* v = static_cast<Variable*>(part->value);
        if (v->type == STRING) {
          TString *toExtract = dynamic_cast<TString *>(v->getValue());
          value = toExtract;
        }
      }
      if(!value) return nullptr;
      int32_t length = value->value.length();
      return new OperatorPart(LITERAL, NUMBER, new TNumber(length), false);
    }
  case NativeCallInterface::READFILE: {
    auto* part = dependencies[0];
    TString* value = nullptr;
    if(part->operatorType == LITERAL) {
      value = static_cast<TString* >(part->value);
    } else if (part->operatorType == VARIABLE) {
      Variable* v = static_cast<Variable*>(part->value);
      if (v->type == STRING) {
        TString *toExtract = dynamic_cast<TString *>(v->getValue());
        value = toExtract;
      }
    }
    if(!value) return nullptr;
    std::fstream stream(value->value);
    stream.seekg (0, stream.end);
    int length = stream.tellg();
    stream.seekg (0, stream.beg);

    // allocate memory:
    char * buffer = new char [length + 1];
    buffer[length ] = '\0';
    // read data as a block:
    stream.read (buffer,length);
    stream.close();
    return new OperatorPart(LITERAL, STRING, new TString(std::string(buffer)), false);

  }
    case NativeCallInterface::WRITEFILE: {
      auto* part = dependencies[0];
      TString* value = nullptr;
      if(part->operatorType == LITERAL) {
        value = static_cast<TString* >(part->value);
      } else if (part->operatorType == VARIABLE) {
        Variable* v = static_cast<Variable*>(part->value);
        if (v->type == STRING) {
          TString *toExtract = dynamic_cast<TString *>(v->getValue());
          value = toExtract;
        }
      }
      if(!value) return nullptr;
      auto* content_part = dependencies[1];
      TString* content_value = nullptr;
      if(content_part->operatorType == LITERAL) {
        content_value = static_cast<TString* >(content_part->value);
      } else if (content_part->operatorType == VARIABLE) {
        Variable* v = static_cast<Variable*>(content_part->value);
        if (v->type == STRING) {
          TString *toExtract = dynamic_cast<TString *>(v->getValue());
          content_value = toExtract;
        }
      }
      if(!content_value) return nullptr;
      std::ofstream stream (value->value);
      stream.write(content_value->value.c_str(), content_value->value.length());
      stream.close();
      return new OperatorPart(LITERAL, BOOLEAN, new TBoolean(true), false);
    }
    default:
      return nullptr;
  }
}

skx::OperatorPart *skx::NativeCallCompiler::compileCall(std::string& content, Context* pContext, CompileItem* pItem) {
//nativecall
  std::string baseOffset = content.substr(11);
  auto split = skx::Utils::split(baseOffset, " ");
  NativeCallInterface::CallType type = skx::NativeCallCompiler::getCallType(split[0]);
  if(type == NativeCallInterface::UNKNOWN) return nullptr;
  std::string args = baseOffset.substr(split[0].length() + 1);
  auto argsSplit = skx::Utils::split(args, " ");
  size_t offset = 0;
  size_t toSkip = 0;
  NativeCallInterface* exec = new NativeCallInterface();
  exec->type = type;
  for(auto& e : argsSplit) {
    if(toSkip > 0) {
      toSkip--;
      continue;
    }
    if (TreeCompiler::isVar(e)) {
      auto* desc = skx::Variable::extractNameSafe(e);
      Variable *propVar = skx::Utils::searchVar(desc, pContext);
      delete desc;
      if(propVar) {
        exec->dependencies.push_back(new OperatorPart(VARIABLE, propVar->type, propVar, propVar->isDouble));
      }
    } else if (e == "true" || e == "false") {
      exec->dependencies.push_back(new OperatorPart(LITERAL, BOOLEAN, new TBoolean(e == "true"), false));
    } else if(TreeCompiler::isNumber(e[0])) {
      exec->dependencies.push_back(skx::Literal::extractNumber(e));
    } else if (e[0] == '"') {
      std::string max = args.substr(offset + 1);
      std::string value = "";

      for(size_t i = 0; i < max.length(); i++) {
        if(max[i] == '"') break;
        if (max[i] == '\\' && i < max.length() - 1 && max[i + 1] == '"') {
          value.append("\"");
          i += 2;
          continue;
        }
        if(max[i] == ' ') {
          toSkip += 1;
        }
        value += max[i];
      }
      exec->dependencies.push_back(new OperatorPart(LITERAL, STRING, new TString(value), false));
    }
    offset += e.length() + 1;
  }
  return new OperatorPart(EXECUTION, UNDEFINED, exec, false);
}
