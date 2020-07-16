//
// Created by liz3 on 16/07/2020.
//

#include "../include/Assembler.h"
#include "../include/Function.h"
#include "../include/types/TNumber.h"
#include "../include/types/TString.h"
#include "../include/types/TBoolean.h"
#include "../include/types/TCharacter.h"
#include "../include/BasicInstructions.h"


void skx::Assembler::assemble(skx::Script *script) {
    std::ofstream stream("out.skx");
    uint8_t ver = 0;
    stream.write("skx", 4);
    stream.write(reinterpret_cast<const char *>(&ver), 1);
    assembleOptions(script->baseContext, &stream);
    uint32_t length = script->signals.size() + script->mc_events.size() + script->mc_commands.size() +
                      script->baseContext->functions.size() + 1;
    stream.write(reinterpret_cast<const char *>(&length), 4);
    for (auto entry : script->baseContext->functions) {
        assembleTree(entry.second->functionItem, 0, &stream, entry.second);
    }
    for (auto entry : script->mc_events) {
        assembleTree(entry.second, 0, &stream, nullptr);
    }
    for (auto entry : script->signals) {
        assembleTree(entry.second, 0, &stream, nullptr);
    }
    stream.close();
}

void skx::Assembler::assembleTree(skx::CompileItem *item, uint32_t counter, std::ofstream *stream, Function *function) {
    if (function != nullptr) {
        uint8_t x = 1;
        stream->write(reinterpret_cast<const char *>(&x), 1);
        stream->write(function->name.c_str(), function->name.length() + 1);
        uint16_t paramSize = function->targetParams.size();
        stream->write(reinterpret_cast<const char *>(&paramSize), 2);
        for (auto &param : function->targetParams) {
            stream->write(param->name.c_str(), param->name.length() + 1);
            uint8_t y = 0;
            stream->write(reinterpret_cast<const char *>(&y), 1);
        }
    } else {
        uint8_t x = 0;
        stream->write(reinterpret_cast<const char *>(&x), 1);
    }
    uint32_t childrenSize = item->children.size();
    stream->write(reinterpret_cast<const char *>(&counter), 4);
    uint32_t triggers = item->triggers.size();
    uint32_t assigments = item->assignments.size();
    uint32_t conditions = item->comparisons.size();
    uint32_t executions = item->executions.size();
    stream->write(reinterpret_cast<const char *>(&triggers), 4);


    for (auto &trigger : item->triggers) {
        assembleTrigger(trigger, stream);
    }
    stream->write(reinterpret_cast<const char *>(&assigments), 4);
    for (auto &assigment : item->assignments) {
        assembleAssigment(assigment, stream);
    }
    stream->write(reinterpret_cast<const char *>(&conditions), 4);
    for (auto &comparison : item->comparisons) {
        assembleComparison(comparison, stream);
    }
    stream->write(reinterpret_cast<const char *>(&executions), 4);
    for (auto &execution : item->executions) {
        assembleExecution(execution, stream);
    }
    stream->write(reinterpret_cast<const char *>(&childrenSize), 4);
    for (auto child : item->children) {
        assembleTree(child, counter + 1, stream, nullptr);
    }
}

void skx::Assembler::assembleTrigger(skx::Trigger *trigger, std::ofstream *stream) {
    uint8_t type = -1;
    switch (trigger->type) {
        case SIGNAL:
            type = 0;
            break;
        case MC_EVENT:
            type = 1;
            break;
        case MC_COMMAND:
            type = 2;
            break;
        default:
            break;
    }
    stream->write(reinterpret_cast<const char *>(&type), 1);
    if (trigger->type == SIGNAL) {
        TriggerSignal *s = dynamic_cast<TriggerSignal *>(trigger);
        uint8_t t = s->signalType == TriggerSignal::LOAD ? 0 : 1;
        stream->write(reinterpret_cast<const char *>(&t), 1);
    }
    if (trigger->type == MC_EVENT) {
        TriggerEvent *ev = dynamic_cast<TriggerEvent *>(trigger);
        stream->write(ev->eventClass.c_str(), ev->eventClass.length() + 1);
    }
    if (trigger->type == MC_COMMAND) {
        TriggerCommand *cm = dynamic_cast<TriggerCommand *>(trigger);
        stream->write(cm->name.c_str(), cm->name.length() + 1);
        // TODO XD
    }
}

void skx::Assembler::assembleAssigment(skx::Assigment *assigment, std::ofstream *stream) {
    uint8_t eqType = -1;
    switch (assigment->type) {
        case ASSIGN:
            eqType = 0;
            break;
        case ADD:
            eqType = 1;
            break;
        case SUBTRACT:
            eqType = 2;
            break;
        case MULTIPLY:
            eqType = 3;
            break;
        case DIVIDE:
            eqType = 4;
            break;
        default:
            break;
    }
    stream->write(reinterpret_cast<const char *>(&eqType), 1);
    if (assigment->source->operatorType == LITERAL) {
        uint8_t sType = 1;
        stream->write(reinterpret_cast<const char *>(&sType), 1);
        uint8_t val = getVarIdFromValue(static_cast<VariableValue *>(assigment->source->value));
        stream->write(reinterpret_cast<const char *>(&val), 1);
        writeValue(static_cast<VariableValue *>(assigment->source->value), stream);
    } else if (assigment->source->operatorType == VARIABLE) {
        uint8_t sType = 0;
        stream->write(reinterpret_cast<const char *>(&sType), 1);
        auto *var = static_cast<Variable *>(assigment->source->value);
        if (var->accessType == CONTEXT) sType = 0;
        if (var->accessType == GLOBAL) sType = 1;
        if (var->accessType == STATIC) sType = 2;
        stream->write(reinterpret_cast<const char *>(&sType), 1);
        sType = var->contextValue ? 1 : 0;
        stream->write(reinterpret_cast<const char *>(&sType), 1);
        stream->write(var->name.c_str(), var->name.length() + 1);
    }
    if (assigment->target->operatorType == LITERAL) {
        uint8_t sType = 1;
        stream->write(reinterpret_cast<const char *>(&sType), 1);
        uint8_t val = getVarIdFromValue(static_cast<VariableValue *>(assigment->target->value));
        stream->write(reinterpret_cast<const char *>(&val), 1);
        writeValue(static_cast<VariableValue *>(assigment->target->value), stream);
    } else if (assigment->target->operatorType == VARIABLE) {
        uint8_t sType = 0;
        stream->write(reinterpret_cast<const char *>(&sType), 1);
        auto *var = static_cast<Variable *>(assigment->target->value);
        if (var->accessType == CONTEXT) sType = 0;
        if (var->accessType == GLOBAL) sType = 1;
        if (var->accessType == STATIC) sType = 2;
        stream->write(reinterpret_cast<const char *>(&sType), 1);
        sType = var->contextValue ? 1 : 0;
        stream->write(reinterpret_cast<const char *>(&sType), 1);
        stream->write(var->name.c_str(), var->name.length() + 1);
    }
}

uint8_t skx::Assembler::getVarIdFromValue(skx::VariableValue *value) {

    switch (value->type) {
        case STRING:
            return 0;
        case BOOLEAN:
            return 1;
        case NUMBER: {
            if (dynamic_cast<TNumber *>(value)->isDouble) {
                return 3;
            } else {
                return 2;
            }
        }
        case CHARACTER:
            return 4;
        case POINTER:
            return 5;
        case UNDEFINED:
            return 6;
        case ARRAY:
            return 7;
    }
    return -1;
}

uint8_t skx::Assembler::writeValue(skx::VariableValue *value, std::ofstream *stream) {

    switch (value->type) {
        case STRING: {
            std::string val = dynamic_cast<TString *>(value)->value;
            stream->write(val.c_str(), val.length() + 1);
            return 0;
        }
        case BOOLEAN: {
            bool val = dynamic_cast<TBoolean *>(value)->value;
            uint8_t intVal = val ? 1 : 0;
            stream->write(reinterpret_cast<const char *>(&intVal), 1);
            return 1;
        }
        case NUMBER: {
            TNumber *val = dynamic_cast<TNumber *>(value);
            if (val->isDouble) {
                stream->write(reinterpret_cast<const char *>(&val->doubleValue), 8);
            } else {
                stream->write(reinterpret_cast<const char *>(&val->intValue), 4);
            }
            return 0;
        }
        case CHARACTER: {
            char val = dynamic_cast<TCharacter *>(value)->value;
            stream->write(reinterpret_cast<const char *>(&val), 1);
        }
        case POINTER:
            if (value->varRef) {
                uint8_t intVal = 1;
                stream->write(reinterpret_cast<const char *>(&intVal), 1);
                std::string type = value->varRef->customTypeName;
                std::string finalValue = value->getStringValue();
                stream->write(type.c_str(), type.length() + 1);
                stream->write(finalValue.c_str(), finalValue.length() + 1);
            } else {
                uint8_t intVal = 0;
                stream->write(reinterpret_cast<const char *>(&intVal), 1);
            }
            return 5;
        case UNDEFINED:
        case ARRAY:
            break;
    }
    return 0;
}

void skx::Assembler::assembleComparison(skx::Comparison *&comparison, std::ofstream *stream) {
    uint8_t eqType = -1;
    switch (comparison->type) {
        case EQUAL:
            eqType = 0;
            break;
        case SMALLER:
            eqType = 1;
            break;
        case BIGGER:
            eqType = 2;
            break;
        case SMALLER_OR_EQUAL:
            eqType = 3;
            break;
        case BIGGER_OR_EQUAL:
            eqType = 4;
            break;
        case NOT_EQUAL:
            eqType = 5;
        default:
            break;
    }
    stream->write(reinterpret_cast<const char *>(&eqType), 1);
    if (comparison->source->operatorType == LITERAL) {
        uint8_t sType = 1;
        stream->write(reinterpret_cast<const char *>(&sType), 1);
        uint8_t val = getVarIdFromValue(static_cast<VariableValue *>(comparison->source->value));
        stream->write(reinterpret_cast<const char *>(&val), 1);
        writeValue(static_cast<VariableValue *>(comparison->source->value), stream);
    } else if (comparison->source->operatorType == VARIABLE) {
        uint8_t sType = 0;
        stream->write(reinterpret_cast<const char *>(&sType), 1);
        auto *var = static_cast<Variable *>(comparison->source->value);
        if (var->accessType == CONTEXT) sType = 0;
        if (var->accessType == GLOBAL) sType = 1;
        if (var->accessType == STATIC) sType = 2;
        stream->write(reinterpret_cast<const char *>(&sType), 1);
        sType = var->contextValue ? 1 : 0;
        stream->write(reinterpret_cast<const char *>(&sType), 1);
        stream->write(var->name.c_str(), var->name.length() + 1);
    }
    if (comparison->target->operatorType == LITERAL) {
        uint8_t sType = 1;
        stream->write(reinterpret_cast<const char *>(&sType), 1);
        uint8_t val = getVarIdFromValue(static_cast<VariableValue *>(comparison->target->value));
        stream->write(reinterpret_cast<const char *>(&val), 1);
        writeValue(static_cast<VariableValue *>(comparison->target->value), stream);
    } else if (comparison->target->operatorType == VARIABLE) {
        uint8_t sType = 0;
        stream->write(reinterpret_cast<const char *>(&sType), 1);
        auto *var = static_cast<Variable *>(comparison->target->value);
        if (var->accessType == CONTEXT) sType = 0;
        if (var->accessType == GLOBAL) sType = 1;
        if (var->accessType == STATIC) sType = 2;
        stream->write(reinterpret_cast<const char *>(&sType), 1);
        sType = var->contextValue ? 1 : 0;
        stream->write(reinterpret_cast<const char *>(&sType), 1);
        stream->write(var->name.c_str(), var->name.length() + 1);
    }

}

void skx::Assembler::assembleOptions(skx::Context *pContext, std::ofstream *stream) {
    uint8_t type = 2;
    stream->write(reinterpret_cast<const char *>(&type), 1);
    uint32_t count = 0;
    std::vector<Variable *> options;
    for (auto &entry : pContext->vars) {
        if (!entry.second->contextValue && entry.second->accessType == STATIC) {
            count++;
            options.push_back(entry.second);
        }
    }
    stream->write(reinterpret_cast<const char *>(&count), 4);
    for (auto var : options) {
        stream->write(var->name.c_str(), var->name.length() + 1);
        uint8_t val = getVarIdFromValue(var->getValue());
        stream->write(reinterpret_cast<const char *>(&val), 1);
        writeValue(var->getValue(), stream);
    }
}

void skx::Assembler::assembleExecution(skx::Execution *&execution, std::ofstream *stream) {
    stream->write(execution->name.c_str(), execution->name.length() + 1);
    uint32_t depsSize = execution->dependencies.size();
    stream->write(reinterpret_cast<const char *>(&depsSize), 4);
    for (auto part : execution->dependencies) {
        if (part->operatorType == LITERAL) {
            uint8_t sType = 1;
            stream->write(reinterpret_cast<const char *>(&sType), 1);
            uint8_t val = getVarIdFromValue(static_cast<VariableValue *>(part->value));
            stream->write(reinterpret_cast<const char *>(&val), 1);
            writeValue(static_cast<VariableValue *>(part->value), stream);
        } else if (part->operatorType == VARIABLE) {
            uint8_t sType = 0;
            stream->write(reinterpret_cast<const char *>(&sType), 1);
            auto *var = static_cast<Variable *>(part->value);
            if (var->accessType == CONTEXT) sType = 0;
            if (var->accessType == GLOBAL) sType = 1;
            if (var->accessType == STATIC) sType = 2;
            stream->write(reinterpret_cast<const char *>(&sType), 1);
            sType = var->contextValue ? 1 : 0;
            stream->write(reinterpret_cast<const char *>(&sType), 1);
            stream->write(var->name.c_str(), var->name.length() + 1);
        }
    }
    if (execution->name == "std::invoke") {
        std::string funcName = dynamic_cast<FunctionInvoker *>(execution)->function->name;
        stream->write(funcName.c_str(), funcName.length() + 1);
    }
    if (execution->name == "std::loop") {
        Loop *loop = dynamic_cast<Loop *>(execution);
        uint8_t loopType = loop->loopTargetVar == nullptr ? 0 : 1;
        stream->write(reinterpret_cast<const char *>(&loopType), 1);
        if (loopType == 0) {
            stream->write(reinterpret_cast<const char *>(&loop->loopTarget), 4);
        } else {
            uint8_t sType = 0;
            auto *var = loop->loopTargetVar;
            if (var->accessType == CONTEXT) sType = 0;
            if (var->accessType == GLOBAL) sType = 1;
            if (var->accessType == STATIC) sType = 2;
            stream->write(reinterpret_cast<const char *>(&sType), 1);
            sType = var->contextValue ? 1 : 0;
            stream->write(reinterpret_cast<const char *>(&sType), 1);
            stream->write(var->name.c_str(), var->name.length() + 1);

        }
    }

}
