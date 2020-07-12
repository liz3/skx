//
// Created by liz3 on 10/07/2020.
//

#include "../../include/api/Json.h"
#include "../../include/utils.h"
#include "../../include/Variable.h"
#include "../../include/Literal.h"
#include "../../include/types/TString.h"
#include "../../include/types/TNumber.h"
#include "../../include/types/TBoolean.h"


skx::OperatorPart *skx::JsonInterface::execute(skx::Context *target) {
    if(type == CREATE_OBJECT || type == CREATE_ARRAY) {
        Variable* targetVar = static_cast<Variable *>(dependencies[0]->value);
        targetVar->type = POINTER;
        targetVar->customTypeName = "data::jsonValue";
        nlohmann::json val = type == CREATE_ARRAY ?  nlohmann::json::array() :  nlohmann::json::object();
        targetVar->setValue( new TJson(val));

        return nullptr;
    }
    if(type == REMOVE) {
        Variable* targetVar = static_cast<Variable *>(dependencies[1]->value);
        if (targetVar->type != POINTER || targetVar->customTypeName != "data::jsonValue") return nullptr;
        TJson* j = dynamic_cast<TJson*>(targetVar->getValue());
        std::string targetProp;
        int32_t extractIndex = 0;
        bool isIndex = false;
        if (dependencies[0]->operatorType == LITERAL) {
            if (dependencies[0]->type != STRING && dependencies[0]->type != NUMBER) return nullptr;
            if (dependencies[0]->type == STRING) {
                TString *toExtract = static_cast<TString *>(dependencies[0]->value);
                targetProp = toExtract->value;
            } else {
                TNumber *toExtract = static_cast<TNumber *>(dependencies[0]->value);
                extractIndex = toExtract->intValue;
                isIndex = true;
            }
        } else if(dependencies[0]->operatorType == VARIABLE) {
            Variable* b = static_cast<Variable*>(dependencies[0]->value);
            if (b->type == STRING) {
                TString *toExtract = dynamic_cast<TString *>(b->getValue());
                targetProp = toExtract->value;
            } else {
                TNumber *toExtract = dynamic_cast<TNumber *>(b->getValue());
                extractIndex = toExtract->intValue;
                isIndex = true;
            }
        }
        if(isIndex) {
            j->value.erase(extractIndex);
        } else {
            j->value.erase(targetProp);
        }
        return nullptr;
    }

    if (type == EXTRACT) {
        Variable *holderVar = static_cast<Variable *>(dependencies[1]->value);
        if (holderVar->type != POINTER || holderVar->customTypeName != "data::jsonValue") return nullptr;
        TJson *v = dynamic_cast<TJson *>(holderVar->getValue());
        std::string targetProp;
        int32_t extractIndex = 0;
        bool isIndex = false;
        if (dependencies[0]->operatorType == LITERAL) {
            if (dependencies[0]->type != STRING && dependencies[0]->type != NUMBER) return nullptr;
            if (dependencies[0]->type == STRING) {
                TString *toExtract = static_cast<TString *>(dependencies[0]->value);
                targetProp = toExtract->value;
            } else {
                TNumber *toExtract = static_cast<TNumber *>(dependencies[0]->value);
                extractIndex = toExtract->intValue;
                isIndex = true;
            }
        } else if(dependencies[0]->operatorType == VARIABLE) {
            Variable* b = static_cast<Variable*>(dependencies[0]->value);
            if (b->type == STRING) {
                TString *toExtract = dynamic_cast<TString *>(b->getValue());
                targetProp = toExtract->value;
            } else {
                TNumber *toExtract = dynamic_cast<TNumber *>(b->getValue());
                extractIndex = toExtract->intValue;
                isIndex = true;
            }
        }
        if (!v->value.contains(targetProp)) return nullptr;
        if (dependencies[2]->operatorType != VARIABLE) return nullptr;
        if (isIndex && (!v->value.is_array() || v->value.size() < extractIndex)) return nullptr;
        nlohmann::json val = isIndex ? v->value[extractIndex] : v->value[targetProp];

        Variable *targetVar = static_cast<Variable *>(dependencies[2]->value);
        if (val.is_array() || val.is_object()) {
            targetVar->type = POINTER;
            targetVar->customTypeName = "data::jsonValue";
//            if(targetVar->getValue() != nullptr && targetVar->getValue()->varRef == targetVar)
//                delete targetVar->getValue();
             targetVar->setValue( new TJson(val));
        } else if (val.is_number()) {
            targetVar->type = NUMBER;
            if (val.is_number_float()) {
                double dVal = val;
                targetVar->setValue(new TNumber(dVal));
            } else {
                int32_t dVal = val;
                targetVar->setValue( new TNumber(dVal));
            }

        } else if (val.is_string()) {
            targetVar->type = STRING;
            std::string sVal = val;
            targetVar->setValue( new TString(sVal));
        }  else if (val.is_boolean()) {
            targetVar->type = BOOLEAN;
            bool sVal = val;
            targetVar->setValue(new TBoolean(sVal));
        } else {
            targetVar->type = UNDEFINED;
            targetVar->setValue(nullptr);
        }
    }
    if(type == PUT) {
        Variable* targetVar = static_cast<Variable*>(dependencies[2]->value);
        if(targetVar->type != POINTER || targetVar->customTypeName != "data::jsonValue")
            return nullptr;
        TJson* targetVal = dynamic_cast<TJson*>(targetVar->getValue());
        std::string targetProp;
        int32_t extractIndex = 0;
        bool isIndex = false;
        {
            if(dependencies[1]->operatorType == LITERAL) {
                isIndex = dependencies[1]->type == NUMBER;
                if(isIndex) {
                    extractIndex = static_cast<TNumber*>(dependencies[1]->value)->intValue;
                } else {
                    targetProp = static_cast<TString*>(dependencies[1]->value)->value;

                }
            } else if(dependencies[1]->operatorType == VARIABLE) {
                Variable* asVar = static_cast<Variable*>(dependencies[1]->value);
                isIndex = asVar->type == NUMBER;
                if(isIndex) {
                    extractIndex = dynamic_cast<TNumber*>(asVar->getValue())->intValue;
                } else {
                    targetProp = dynamic_cast<TString*>(asVar->getValue())->value;

                }
            }
        }
        VariableValue* sourceVal;
        VarType type = UNDEFINED;
        std::string customType;
        if(dependencies[0]->operatorType == LITERAL) {
            sourceVal = static_cast<VariableValue*>(dependencies[0]->value);
            type = dependencies[0]->type;
        } else if (dependencies[0]->operatorType == VARIABLE) {
            Variable* v = static_cast<Variable*>(dependencies[0]->value);
            sourceVal = v->getValue();
            customType = v->customTypeName;
            type = v->type;
        }
        switch (type) {
            case STRING: {
                std::string v = dynamic_cast<TString*>(sourceVal)->value;
                isIndex ? targetVal->value[extractIndex] = v : targetVal->value[targetProp] = v;
                break;
            }
            case NUMBER: {
                TNumber* v = dynamic_cast<TNumber*>(sourceVal);
                if(v->isDouble) {
                    isIndex ? targetVal->value[extractIndex] = v->doubleValue : targetVal->value[targetProp] = v->doubleValue;
                } else {
                    isIndex ? targetVal->value[extractIndex] = v->intValue : targetVal->value[targetProp] = v->intValue;
                }
                break;
            }
            case BOOLEAN: {
                TBoolean* v = dynamic_cast<TBoolean*>(sourceVal);
                isIndex ? targetVal->value[extractIndex] = v->value : targetVal->value[targetProp] = v->value;
                break;
            }
            case POINTER: {
               if(customType == "data::jsonValue") {
                   TJson *val = dynamic_cast<TJson*>(sourceVal);
                   isIndex ? targetVal->value[extractIndex] = val->value : targetVal->value[targetProp] = val->value;
               }
                break;
            }
            default:
                break;
        }
    }
    if (type == PARSE) {
        std::string sourceVal;
        if (dependencies[0]->operatorType == LITERAL) {
            sourceVal = static_cast<TString *>(dependencies[0]->value)->value;
        } else if (dependencies[0]->operatorType == VARIABLE) {
            Variable *var = static_cast<Variable *>(dependencies[0]->value);
            sourceVal = dynamic_cast<TString *>(var->getValue())->value;
        }
        Variable *targetVar = static_cast<Variable *>(dependencies[1]->value);
        nlohmann::json parsed = nlohmann::json::parse(sourceVal);
        targetVar->type = POINTER;
        targetVar->customTypeName = "data::jsonValue";
        targetVar->setValue( new TJson(parsed));
    }
    if (type == STRINGIFY) {
        nlohmann::json sourceVal;
        if (dependencies[0]->operatorType == VARIABLE) {
            Variable *s = static_cast<Variable *>(dependencies[0]->value);
            if (s->type != POINTER || s->customTypeName != "data::jsonValue") return nullptr;
            sourceVal = dynamic_cast<TJson *>(s->getValue())->value;
        } else {
            return nullptr;
        }
        Variable *targetVar = static_cast<Variable *>(dependencies[1]->value);
        targetVar->type = STRING;
        targetVar->customTypeName = "";
        targetVar->setValue(new TString(sourceVal.dump()));
    }
    return nullptr;
}

void skx::Json::compileRequest(std::string &content, skx::Context *ctx, skx::CompileItem *target) {
    auto split = skx::Utils::split(content, " ");
    if (split.size() < 2 || split[1] != "json") return;

    if (split[0] == "parse") {
        // parse json "{.....}" to {_jsonData}
        JsonInterface *exec = new JsonInterface();
        exec->type = JsonInterface::PARSE;
        if (split[2].rfind('"', 0) == 0) {
            std::string json = "";
            std::string start = content.substr(12);
            for (int i = 0; i < start.length(); ++i) {
                if (start[i] == '"') break;
                if (start[i] == '\\' && i < start.length() - 1 && start[i + 1] == '"') {
                    json.append("\"");
                    i += 2;
                    continue;
                }
                json += start[i];
            }
            exec->dependencies.push_back(new OperatorPart(LITERAL, STRING, new TString(json), false));
        } else {
            auto* desc = skx::Variable::extractNameSafe(split[2]);
            Variable *var = skx::Utils::searchVar(desc, ctx);
            delete desc;
            if (!var) {
                return;
            }
            exec->dependencies.push_back(new OperatorPart(VARIABLE, STRING, var, false));
        }
        auto targetName = split[split.size() - 1];
        auto *descriptor = skx::Variable::extractNameSafe(targetName);
        Variable *var = skx::Utils::searchVar(descriptor, ctx);
        if (!var) {
            Context *c = descriptor->type == CONTEXT ? ctx : ctx->global;
            var = new Variable();
            var->name = descriptor->name;
            var->type = POINTER;
            var->accessType = descriptor->type;
            var->ctx = c;
            c->vars[descriptor->name] = var;
        }
        exec->dependencies.push_back(new OperatorPart(VARIABLE, POINTER, var, false));
        target->executions.push_back(exec);
        delete descriptor;
    }
    if (split[0] == "stringify") {
        //stringify json {x} to {wtf}
        JsonInterface *exec = new JsonInterface();
        exec->type = JsonInterface::STRINGIFY;
        {
            auto* desc = skx::Variable::extractNameSafe(split[2]);
            Variable *var = skx::Utils::searchVar(desc, ctx);
            delete desc;
            if (!var) {
                return;
            }
            exec->dependencies.push_back(new OperatorPart(VARIABLE, STRING, var, false));
        }
        auto targetName = split[split.size() - 1];
        auto *descriptor = skx::Variable::extractNameSafe(targetName);
        Variable *var = skx::Utils::searchVar(descriptor, ctx);
        if (!var) {
            Context *c = descriptor->type == CONTEXT ? ctx : ctx->global;
            var = new Variable();
            var->name = descriptor->name;
            var->type = POINTER;
            var->accessType = descriptor->type;
            var->ctx = c;
            c->vars[descriptor->name] = var;
        }
        delete descriptor;
        exec->dependencies.push_back(new OperatorPart(VARIABLE, STRING, var, false));
        target->executions.push_back(exec);
    } else if (split[0] == "create") {
        //create json array as {wtf}
        JsonInterface *exec = new JsonInterface();
        if (split[2] == "array") {
            exec->type = JsonInterface::CREATE_ARRAY;
        } else if (split[2] == "object") {
            exec->type = JsonInterface::CREATE_OBJECT;
        }
        auto targetName = split[split.size() - 1];
        auto *descriptor = skx::Variable::extractNameSafe(targetName);
        Variable *var = skx::Utils::searchVar(descriptor, ctx);
        if (!var) {
            Context *c = descriptor->type == CONTEXT ? ctx : ctx->global;
            var = new Variable();
            var->name = descriptor->name;
            var->type = POINTER;
            var->accessType = descriptor->type;
            var->ctx = c;
            c->vars[descriptor->name] = var;
        }
        delete descriptor;
        exec->dependencies.push_back(new OperatorPart(VARIABLE, POINTER, var, false));
        target->executions.push_back(exec);
    }  else if (split[0] == "remove") {
        //remove json value "name/index" from {object}
        JsonInterface *exec = new JsonInterface();
        exec->type = JsonInterface::REMOVE;
        auto property = split[3];
        auto targetName = split[5];
        if (TreeCompiler::isVar(property)) {
            Variable *propVar = skx::Utils::searchVar(skx::Variable::extractNameSafe(property), ctx);
            if (!propVar) return;
            exec->dependencies.push_back(new OperatorPart(VARIABLE, propVar->type, propVar, propVar->isDouble));
        } else if (TreeCompiler::isNumber(property[0])) {
            exec->dependencies.push_back(skx::Literal::extractNumber(property));

        } else {
            exec->dependencies.push_back(
                    new OperatorPart(LITERAL, STRING, new TString(property.substr(1, property.length() - 2)), false));
        }
        auto *descriptor = skx::Variable::extractNameSafe(targetName);
        Variable *var = skx::Utils::searchVar(descriptor, ctx);
        delete descriptor;
        if (!var) {
            return;
        }
        exec->dependencies.push_back(new OperatorPart(VARIABLE, POINTER, var, false));
        target->executions.push_back(exec);
    }  else if (split[0] == "extract") {
        // extract json value "myfield" from {_jsonData} to {_newValue}
        JsonInterface *exec = new JsonInterface();
        exec->type = JsonInterface::EXTRACT;
        auto property = split[3];
        auto from = split[5];
        auto to = split[7];
        if (property.rfind('{', 0) == 0) {
            auto* desc = skx::Variable::extractNameSafe(property);
            Variable *propVar = skx::Utils::searchVar(desc, ctx);
            delete desc;
            if (!propVar) return;
            exec->dependencies.push_back(new OperatorPart(VARIABLE, propVar->type, propVar, propVar->isDouble));
        } else if (TreeCompiler::isNumber(property[0])) {
            exec->dependencies.push_back(skx::Literal::extractNumber(property));

        } else {
            exec->dependencies.push_back(
                    new OperatorPart(LITERAL, STRING, new TString(property.substr(1, property.length() - 2)), false));
        }
        auto* desc = skx::Variable::extractNameSafe(from);
        Variable *fromVar = skx::Utils::searchVar(desc, ctx);
        delete desc;
        if (!fromVar) return;
        exec->dependencies.push_back(new OperatorPart(VARIABLE, POINTER, fromVar, false));
        auto *descriptor = skx::Variable::extractNameSafe(to);
        Variable *var = skx::Utils::searchVar(descriptor, ctx);
        if (!var) {
            Context *c = descriptor->type == CONTEXT ? ctx : ctx->global;
            var = new Variable();
            var->name = descriptor->name;
            var->type = UNDEFINED;
            var->accessType = descriptor->type;
            var->ctx = c;
            c->vars[descriptor->name] = var;
        }
        exec->dependencies.push_back(new OperatorPart(VARIABLE, POINTER, var, false));
        target->executions.push_back(exec);
        delete descriptor;

    } else if (split[0] == "put") {
        // put json value {sourceVal} as "name/index" in {jsonObject}
        JsonInterface *exec = new JsonInterface();
        exec->type = JsonInterface::PUT;
        auto property = split[3];
        auto asVal = split[split.size() - 3];
        auto to = split[split.size() - 1];
        {
            if (TreeCompiler::isVar(property)) {
                auto* desc = skx::Variable::extractNameSafe(property);
                Variable *propVar = skx::Utils::searchVar(desc, ctx);
                delete desc;
                if (!propVar) return;
                exec->dependencies.push_back(new OperatorPart(VARIABLE, propVar->type, propVar, propVar->isDouble));
            } else if (TreeCompiler::isNumber(property[0])) {
                exec->dependencies.push_back(skx::Literal::extractNumber(property));

            } else {
                std::string json = "";
                std::string start = content.substr(16);
                for (int i = 0; i < start.length(); ++i) {
                    if (start[i] == '"') break;
                    if (start[i] == '\\' && i < start.length() - 1 && start[i + 1] == '"') {
                        json.append("\"");
                        i += 2;
                        continue;
                    }
                    json += start[i];
                }
                exec->dependencies.push_back(new OperatorPart(LITERAL, STRING, new TString(json), false));
            }

        }
        {
            if (TreeCompiler::isVar(asVal)) {
                auto* desc = skx::Variable::extractNameSafe(asVal);
                Variable *propVar = skx::Utils::searchVar(desc, ctx);
                delete desc;
                if (!propVar) return;
                exec->dependencies.push_back(new OperatorPart(VARIABLE, propVar->type, propVar, propVar->isDouble));
            } else if (TreeCompiler::isNumber(asVal[0])) {
                exec->dependencies.push_back(skx::Literal::extractNumber(asVal));

            } else {
                exec->dependencies.push_back(
                        new OperatorPart(LITERAL, STRING, new TString(asVal.substr(1, asVal.length() - 2)), false));
            }
        }
        {
            auto* desc = skx::Variable::extractNameSafe(to);
            Variable *fromVar = skx::Utils::searchVar(desc, ctx);
            delete desc;
            if (!fromVar) return;
            exec->dependencies.push_back(new OperatorPart(VARIABLE, POINTER, fromVar, false));
        }
        target->executions.push_back(exec);
    }


}

skx::TJson::TJson(const nlohmann::json &value) : value(value) {
    type = POINTER;
}

skx::TJson::TJson() {
    type = POINTER;
}

skx::TJson::~TJson() {

}

skx::VariableValue *skx::TJson::copyValue() {
    return this;
}
