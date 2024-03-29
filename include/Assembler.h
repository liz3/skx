//
// Created by liz3 on 16/07/2020.
//

#ifndef SKX_ASSEMBLER_H
#define SKX_ASSEMBLER_H
#include "Script.h"
#include "Function.h"
#include "TreeCompiler.h"
#include <fstream>
#include <vector>
#include <map>
#include "Variable.h"
#include "types/TString.h"
#include "types/TNumber.h"
#include <string>
#include <iostream>
namespace skx {
struct VariablePos {
  int64_t offset;
  Variable* var;
  std::string pushName;
};
class AssemblySection {
private:
  std::vector<std::string> segments;

public:
  std::string name;
  AssemblySection(std::string name) : name(name) {
  }
  void add(std::string str) {
    segments.push_back(str);
  }
  std::string serialise() {
    std::string output;
    output.append(name + ":\n");
    for(auto& line : segments) {
      output.append("  " + line + "\n");
    }
    return output;
  }
};
class AssemblerContext {
public:
  AssemblerContext(AssemblerContext* parent) : parent(parent) {

  }
  int64_t stack = 0;
  AssemblerContext* parent = nullptr;
  std::vector<VariablePos> vars;
  std::string add(Variable* var, std::string pushName = "") {
    std::string exists = getFor(var);
    if(exists.length() != 0)
       return exists;
    stack += 8;
    inc();
    VariablePos pos;
    pos.var = var;
    pos.offset = 0;
    pos.pushName = pushName;
    vars.push_back(pos);

    return "[sp, #" + std::to_string(pos.offset) + "]";

  }
  void inc() {
    for(auto& entry : vars) {
      entry.offset += 8;
    }
    if(parent != nullptr)
      parent->inc();

  }
  std::string getFor(Variable* var) {

    for(auto& entry : vars) {
      if(entry.var == var) {
        int64_t diff = entry.offset;
        return "[sp, #" + std::to_string(diff) + "]";
      }
    }
    if(parent == nullptr)
      return "";
    return parent->getForWithOffset(var, stack);
  }
  std::string getForWithOffset(Variable* var, int64_t offset) {

    for(auto& entry : vars) {
      if(entry.var == var) {
        int64_t diff = (entry.offset) + offset;
        return "[sp, #" + std::to_string(diff) + "]";
      }
    }
    if(parent == nullptr)
      return "";
    return parent->getForWithOffset(var, offset + stack);
  }

  std::string getSize(bool drop) {
    int64_t size = 2;
    for(auto& entry : vars) {
      if(entry.pushName.length() == 0)
        size++;
    }
    int remaining = size % 4;
    if(remaining != 0)
      size += (4 - remaining);
    std::string savedRegs =  (std::string(!drop ? "stp" : "ldp") + " x29, x30, [sp, #") + std::to_string((size * 8)-16)+"]";
    std::string numStr =  (drop ? "add sp, sp, #" : "sub sp, sp, #") + std::to_string(size * 8);
    return drop ? savedRegs + "\n\t" + numStr : numStr + "\n\t" + savedRegs;
  }
  std::vector<std::string> getStackAdds(bool swap) {
    std::vector<std::string> adds;
    return adds;
  }

};
struct DataEntry {
  std::string name;
  std::string value;
  VariableValue* var;
};
class DataSection {
  private:
  std::map<VariableValue*, DataEntry> entries;
  std::map<std::string, std::string> statics;
  size_t count = 0;

  public:
  std::string getOrRegisterStatic(std::string name, std::string value) {
    if(!statics.count(name))
      statics[name] = value;
    return name;
  }
  std::string getOrRegister(VariableValue* var) {
    if(entries.count(var))
      return entries[var].name;
    size_t id = count++;
    std::string name = "str_" + std::to_string(id);
    TString* str = static_cast<TString*>(var);
    DataEntry entry;
    entry.name = name;
    entry.value = str->value;
    entry.var = var;
    entries[var] = entry;
    return name;
  }
  std::string serialise() {
    std::string output;
    for(auto& entry : entries) {
      output.append(entry.second.name + ":\n\t.ascii \"" + entry.second.value +  "\\0\"\n");
      output.append(entry.second.name + "_len = . - " + entry.second.name + "\n");
    }
    for(auto& entry : statics) {
      output.append(entry.first + ":\n\t.ascii \"" + entry.second +  "\\0\"\n");
      output.append(entry.first + "_len = . - " + entry.first + "\n");
    }

    return output;
  }
};
class Assembler {
 public:
  static std::string assemble(Script* script);
  DataSection data;
  std::map<std::string, AssemblySection*> sections;
  size_t sectionCount = 0;

 private:
  std::string getOpFromType(InstructionOperator type);
  std::string getNameForFunctionParameter(size_t index);
  void assembleTree(CompileItem* item, AssemblerContext* ctx, AssemblySection* section, bool omitStackSize = false);
  void walk(CompileItem* item, AssemblerContext* ctx, AssemblySection* section);
  void assembleTreeWithName(CompileItem* item, AssemblerContext* ctx, std::string name);
  void assembleFunction(Function* function, AssemblerContext* ctx);
  void reportVars(CompileItem* ctx, AssemblerContext* asCtx);

  AssemblySection* getSection(std::string name);
};
}


#endif //SKX_ASSEMBLER_H
