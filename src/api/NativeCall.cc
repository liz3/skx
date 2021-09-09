#include "../../include/api/NativeCall.h"
#include "../../include/utils.h"
#include "../../include/Literal.h"
#include "../../include/types/TString.h"
#include "../../include/types/TMap.h"
#include "../../include/types/TNumber.h"
#include "../../include/types/TBoolean.h"


#include <math.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <regex>
#include "../../include/Script.h"
#include "../../include/Executor.h"


skx::NativeCallInterface::CallType skx::NativeCallCompiler::getCallType(std::string &entry) {

  if(entry == "argslen") return NativeCallInterface::ARGS_LEN;
  if(entry == "args") return NativeCallInterface::ARGS;
  if(entry == "getline") return NativeCallInterface::GET_LINE;

  if(entry == "readfile") return NativeCallInterface::READFILE;
  if(entry == "writefile") return NativeCallInterface::WRITEFILE;
  if(entry == "removefile") return NativeCallInterface::REMFILE;

  if(entry == "getenv") return NativeCallInterface::GETENV;

  if(entry == "strlen") return NativeCallInterface::STRLEN;
  if(entry == "strsplit") return NativeCallInterface::STRING_SPLIT;

  if(entry == "maplen" || entry == "mapsize") return NativeCallInterface::MAP_SIZE;

  if(entry == "mathpow") return NativeCallInterface::MATH_POW;
  if(entry == "mathsin") return NativeCallInterface::MATH_SIN;
  if(entry == "mathcos") return NativeCallInterface::MATH_COS;
  if(entry == "mathtan") return NativeCallInterface::MATH_TAN;
  if(entry == "mathsqrt") return NativeCallInterface::MATH_SQRT;
  if(entry == "mathfloor") return NativeCallInterface::MATH_FLOOR;
  if(entry == "mathceil") return NativeCallInterface::MATH_CEIL;

  if(entry == "eval" || entry == "evaluate") return NativeCallInterface::EVAL;

  if(entry == "writestream" || entry == "writeio") return NativeCallInterface::WRITE_OUT;

  return NativeCallInterface::UNKNOWN;
}

skx::OperatorPart *skx::NativeCallInterface::execute(skx::Context *target) {
  switch(type) {
  case NativeCallInterface::MAP_SIZE: {
    auto* part = dependencies[0];
    if(part->operatorType != VARIABLE)
      return nullptr;
    Variable* v = static_cast<Variable*>(part->value);
    int32_t val = 0;
    if(v->type == MAP) {
      TMap* map = static_cast<TMap*>(v->getValue());
      val = map->value.size();
    }
    return new OperatorPart(LITERAL, NUMBER, new TNumber(val), false);
  }
  case NativeCallInterface::ARGS_LEN: {
    return new OperatorPart(LITERAL, NUMBER, new TNumber((int32_t)skx::Script::argsLength), false);
  }
  case NativeCallInterface::GET_LINE: {
    std::string line;
    std::getline(std::cin, line);
    return new OperatorPart(LITERAL, STRING, new TString(line), false);
  }
  case NativeCallInterface::ARGS: {
    auto* part = dependencies[0];
    TNumber* value = nullptr;
    if(part->operatorType == LITERAL) {
      value = static_cast<TNumber*>(part->value);
    } else if (part->operatorType == VARIABLE) {
      Variable* v = static_cast<Variable*>(part->value);
      if(v->type == NUMBER) {
        TNumber *toExtract = dynamic_cast<TNumber *>(v->getValue());
        value = toExtract;
      }
    }
    if(!value) return nullptr;
    return new OperatorPart(LITERAL, STRING, new TString(skx::Script::args[value->intValue]), false);
  }
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
  case NativeCallInterface::EVAL: {
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
    bool auto_wrap = true;
    if(dependencies.size() == 2) {

      auto* aw_part = dependencies[1];
      if(aw_part->operatorType == LITERAL) {
        auto_wrap = static_cast<TBoolean* >(aw_part->value)->value;
      } else if (aw_part->operatorType == VARIABLE) {
        Variable* v = static_cast<Variable*>(aw_part->value);
        if (v->type == BOOLEAN) {
          TBoolean *toExtract = dynamic_cast<TBoolean *>(v->getValue());
          auto_wrap = toExtract->value;
        }
      }
    }
    std::string code = value->value;
    if(auto_wrap) {
      code = "on load:\n  " + code + "\n";
    }
    code = std::regex_replace(code, std::regex("\\\\t"), "\t");
    code = std::regex_replace(code, std::regex("\\\\n"), "\n");
    auto compile_result = skx::Script::parse(code.c_str());
    for(const auto& entry : compile_result->signals) {
      if(entry.first->signalType == skx::TriggerSignal::LOAD) {
        skx::Executor::executeStandalone(entry.second);
      }
    }
    for(const auto& entry : compile_result->signals) {
      if(entry.first->signalType == skx::TriggerSignal::UN_LOAD) {
        skx::Executor::executeStandalone(entry.second);
      }
    }

    delete compile_result;


    return new OperatorPart(LITERAL, BOOLEAN, new TBoolean(true), false);
  }

  case NativeCallInterface::GETENV: {
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
    const char* envVal = std::getenv(value->value.c_str());
    if(envVal == nullptr) return nullptr;
    return new OperatorPart(LITERAL, STRING, new TString(std::string(envVal)), false);
  }
  case NativeCallInterface::WRITE_OUT: {
    auto* streamPart = dependencies[0];
    TNumber* stream = nullptr;
    if(streamPart->operatorType == LITERAL) {
      stream = static_cast<TNumber*>(streamPart->value);
    } else if (streamPart->operatorType == VARIABLE) {
      Variable* v = static_cast<Variable*>(streamPart->value);
      if (v->type == NUMBER) {
        TNumber *toExtract = dynamic_cast<TNumber *>(v->getValue());
        stream = toExtract;
      }
    }
    auto* contentPart = dependencies[1];
    TString* content = nullptr;
    if(contentPart->operatorType == LITERAL) {
      content = static_cast<TString*>(contentPart->value);
    } else if (contentPart->operatorType == VARIABLE) {
      Variable* v = static_cast<Variable*>(contentPart->value);
      if (v->type == NUMBER) {
        TString *toExtract = dynamic_cast<TString *>(v->getValue());
        content = toExtract;
      }
    }
    if(!content)
      return nullptr;
    int32_t res = 0;
    Context* g = target->global;
    if(g->printFunc != nullptr)
      g->printFunc(content->value, stream->intValue);
    else
      res = write(stream->intValue, content->value.c_str(), content->value.length());


    return new OperatorPart(LITERAL, NUMBER, new TNumber(res), false);
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
    if(!stream || !stream.is_open()) {
      return nullptr;
    }
    stream.seekg (0, stream.end);
    int length = stream.tellg();
    stream.seekg (0, stream.beg);

    // allocate memory:
    char * buffer = new char [length + 1];
    buffer[length ] = '\0';
    // read data as a block:
    stream.read (buffer,length);
    stream.close();
    auto* res = new OperatorPart(LITERAL, STRING, new TString(std::string(buffer)), false);
    delete[] buffer;
    return res;

  }
 case NativeCallInterface::REMFILE: {
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
    int32_t result = remove(value->value.c_str());
    return new OperatorPart(LITERAL, NUMBER, new TNumber(result), false);

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
  case STRING_SPLIT: {
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
    auto split_parts = skx::Utils::split(content_value->value, value->value);
    std::vector<skx::MapEntry> entries;
    for(size_t i = 0; i < split_parts.size(); i++) {
      std::string current = split_parts[i];
      std::string index_str = std::to_string(i);
      TString* stre = new TString(current);
      entries.push_back(MapEntry{index_str, stre});
    }
    auto* p =  new OperatorPart(LITERAL, MAP, new TMap(entries), false);
    p->isList = true;
    return p;
  }
    // Math stuff
  case MATH_POW: {
    auto* part = dependencies[0];
    TNumber* value = nullptr;
    if(part->operatorType == LITERAL) {
      value = static_cast<TNumber* >(part->value);
    } else if (part->operatorType == VARIABLE) {
      Variable* v = static_cast<Variable*>(part->value);
      if (v->type == NUMBER) {
        TNumber *toExtract = dynamic_cast<TNumber *>(v->getValue());
        value = toExtract;
      }
    }
    if(!value) return nullptr;
    auto* content_part = dependencies[1];
    TNumber* content_value = nullptr;
    if(content_part->operatorType == LITERAL) {
      content_value = static_cast<TNumber* >(content_part->value);
    } else if (content_part->operatorType == VARIABLE) {
      Variable* v = static_cast<Variable*>(content_part->value);
      if (v->type == NUMBER) {
        TNumber *toExtract = dynamic_cast<TNumber *>(v->getValue());
        content_value = toExtract;
      }
    }
    if(!content_value) return nullptr;
    double result = pow(value->isDouble ? value->doubleValue : value->intValue, content_value->isDouble ? content_value->doubleValue : content_value->intValue);
    return new OperatorPart(LITERAL, NUMBER, new TNumber(result), true);

  }
  case MATH_COS:
  case MATH_TAN:
  case MATH_SQRT:
  case MATH_SIN: {
    auto* part = dependencies[0];
    TNumber* value = nullptr;
    if(part->operatorType == LITERAL) {
      value = static_cast<TNumber* >(part->value);
    } else if (part->operatorType == VARIABLE) {
      Variable* v = static_cast<Variable*>(part->value);
      if (v->type == NUMBER) {
        TNumber *toExtract = dynamic_cast<TNumber *>(v->getValue());
        value = toExtract;
      }
    }
    if(!value) return nullptr;
    double result;
    if(type == MATH_SIN) {
      result = sin(value->isDouble ? value->doubleValue : value->intValue);
    } else if (type == MATH_COS) {
      result = cos(value->isDouble ? value->doubleValue : value->intValue);
    } else if (type == MATH_TAN) {
      result = tan(value->isDouble ? value->doubleValue : value->intValue);
    } else if (type == MATH_SQRT) {
      result = sqrt(value->isDouble ? value->doubleValue : value->intValue);
    } else {
      return nullptr;
    }
    return new OperatorPart(LITERAL, NUMBER, new TNumber(result), true);
  }
  case MATH_CEIL:
  case MATH_FLOOR: {
    auto* part = dependencies[0];
    TNumber* value = nullptr;
    if(part->operatorType == LITERAL) {
      value = static_cast<TNumber* >(part->value);
    } else if (part->operatorType == VARIABLE) {
      Variable* v = static_cast<Variable*>(part->value);
      if (v->type == NUMBER) {
        TNumber *toExtract = dynamic_cast<TNumber *>(v->getValue());
        value = toExtract;
      }
    }
    if(!value) return nullptr;
    int32_t result;
    if(type == MATH_CEIL) {
      result = ceil(value->isDouble ? value->doubleValue : value->intValue);
    } else if (type == MATH_FLOOR) {
      result = floor(value->isDouble ? value->doubleValue : value->intValue);
    } else {
      return nullptr;
    }
    return new OperatorPart(LITERAL, NUMBER, new TNumber(result), false);


  }


  default:
    return nullptr;
  }
}

skx::NativeCallInterface *skx::NativeCallCompiler::compileCall(std::string& content, Context* pContext, CompileItem* pItem) {
//nativecall
  std::string baseOffset = content.substr(11);
  auto split = skx::Utils::split(baseOffset, " ");
  NativeCallInterface::CallType type = skx::NativeCallCompiler::getCallType(split[0]);
  if(type == NativeCallInterface::UNKNOWN) return nullptr;
  NativeCallInterface* exec = new NativeCallInterface();
  exec->type = type;
  if (split.size() == 1)
    return exec;
  std::string args = baseOffset.substr(split[0].length() + 1);
  auto argsSplit = skx::Utils::split(args, " ");
  size_t offset = 0;
  size_t toSkip = 0;
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
      value = Utils::unescape(value);
      exec->dependencies.push_back(new OperatorPart(LITERAL, STRING, new TString(value), false));
    }
    offset += e.length() + 1;
  }
  return exec;
}
