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
    struct VariableDescriptor {
        std::string name;
        AccessType type;
        bool isFromContext = false;

    };
    class VariableValue {
    public:
        virtual ~VariableValue();

        bool isReadOnly = false;
        VarType type;
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
        VariableValue* value;
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
        static void createVarFromOption(std::string item, skx::Context* targetContext, bool isStatic);
        static VariableDescriptor* extractNameSafe(std::string in);
        static void createVarValue(VarType type, Variable* target, bool isDouble = false);
        VarState state = SPOILED;
        bool contextValue = false;
        bool created = false;
    };


}


#endif //SKX_VARIABLE_H
