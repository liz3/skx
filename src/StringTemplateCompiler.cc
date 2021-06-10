#include "../../include/StringTemplateCompiler.h"
#include <vector>
#include <iostream>

namespace skx {
 OperatorPart* StringTemplateCompiler::compile(std::string baseStr, Context* ctx, CompileItem* target) {
   std::vector<StringTemplate::TemplateEntry> baseParts;
   std::vector<OperatorPart*> deps;
   std::string baseCpy = baseStr;
   while(true) {
     int index = baseCpy.find("%", 0);
     if(index == std::string::npos) {
       baseParts.push_back(StringTemplate::TemplateEntry{baseCpy, 0});
       break;
     }
     if(index > 0 && baseCpy[index-1] == '\\') {
       baseParts.push_back(StringTemplate::TemplateEntry{baseCpy.substr(0, index-1) + "%", 0});
       baseCpy = baseCpy.substr(index+1);
       continue;
     }
     if(index > 0) {
       baseParts.push_back(StringTemplate::TemplateEntry{baseCpy.substr(0, index), 0});
       baseCpy = baseCpy.substr(index);
     }
     int end = baseCpy.find("%", 1);
     if(end == std::string::npos) return nullptr; // encountered an error
     std::string content = baseCpy.substr(0, end+1);
     auto* descriptor = Variable::extractNameSafe(content, true);
     Variable *var = skx::Utils::searchVar(descriptor, ctx);
     delete descriptor;
     if(!var) {
        std::cout << "[WARNING] Template Variable not found: " << descriptor->name << " at: " << target->line
                  << "\n";

        return nullptr;

     }
     deps.push_back(new OperatorPart(VARIABLE, var->type, var, var->isDouble));
     baseParts.push_back(StringTemplate::TemplateEntry{"", 1});
  //   std::cout << content << ":" << descriptor->name << "\n";

     baseCpy = baseCpy.substr(end+1);
   }
   if(deps.size() == 0)
     return nullptr;
   auto* exec = new StringTemplate();
   exec->dependencies = deps;
   exec->baseParts = baseParts;
   return new OperatorPart(EXECUTION, UNDEFINED, exec, false);
 }

}
