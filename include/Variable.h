//
// Created by liz3 on 29/06/2020.
//

#ifndef SKX_VARIABLE_H
#define SKX_VARIABLE_H

#include "Context.h"
#include <string>


namespace skx {
class Context;
enum VarType {
  STRING,
  NUMBER,
  ARRAY,
  CHARACTER,
  POINTER,
  BOOLEAN,
  MAP,
  UNDEFINED
};

enum AccessType {
  STATIC,
  CONTEXT,
  GLOBAL
};
enum VarState {
  SPOILED,
  RUNTIME_CR
};
enum ListAccessorType {
  ALL,
  VAR_VALUE
};
struct ListAccessor {
  std::string name;
  ListAccessorType type;
};
struct VariableDescriptor {
  std::string name;
  ListAccessor* listAccessor = nullptr;
  AccessType type;
  bool isFromContext = false;

};
class VariableValue {
 public:
  virtual ~VariableValue();

  bool isReadOnly = false;
  VarType type;
  std::string customTypeName = "";
  Variable* varRef = nullptr;
  //comparison
  virtual bool isEqual(VariableValue* other);
  virtual bool isSmaller(VariableValue* other);
  virtual bool isBigger(VariableValue* other);
  virtual bool isSmallerOrEquals(VariableValue* other);
  virtual bool isBiggerOrEquals(VariableValue* other);

  //assignments
  virtual bool assign(VariableValue* source);
  virtual bool add(VariableValue* source);
  virtual bool subtract(VariableValue* source);
  virtual bool multiply(VariableValue* source);
  virtual bool divide(VariableValue* source);
  virtual std::string getStringValue();
  virtual VariableValue* copyValue();
};
class Variable {
 private:
  VariableValue* value = nullptr;
 public:
  VariableValue *getValue() const;

  void setValue(VariableValue *value);

 public:
  virtual ~Variable();
  AccessType accessType;
  VarType type;
  Context* ctx;
  std::string name;
  std::string customTypeName; // when pointer
  bool isDouble = false; // specific to number;
  static void createVarFromOption(std::string item, skx::Context *targetContext);
  static VariableDescriptor* extractNameSafe(std::string in, bool isFromTemplate = false);
  static void createVarValue(VarType type, Variable* target, bool isDouble = false);
  static VariableValue* extractValueFromList(Variable* map, Variable* index);
  VarState state = SPOILED;
  bool contextValue = false;
  bool created = false;
};


}


#endif //SKX_VARIABLE_H
