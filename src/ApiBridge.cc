//
// Created by liz3 on 11/07/2020.
//
#ifdef SKX_BUILD_API
#include <iostream>
#include "../include/ApiBridge.h"
#include <net_liz3_skx_SkxApi.h>
#include "../include/types/TString.h"

namespace skx {
    skx::ApiBridge *apiInstance = nullptr;

    ApiBridge::ApiBridge(JNIEnv_ *pEnv, _jobject *pJobject) {
        this->env = pEnv;
        this->thisInstance = env->NewGlobalRef(pJobject);
        this->classType = env->FindClass("net/liz3/skx/SkxApi");
        this->eventHookClassType = env->FindClass("net/liz3/skx/include/EventHook");
    }


    bool ApiBridge::setup() {
        //  setStringValue("apiErrorMessage", "this is a test error");
        return true;
    }

    bool ApiBridge::shutdown() {
        for (auto executor : executors) {
            executor.second->interrupt();
            delete executor.second;
        }
        executors.clear();
        for (const auto *entry : scripts) {
            delete entry;
        }
        scripts.clear();
        hooks.clear();
        return true;
    }

    void ApiBridge::setStringValue(const char *fieldName, const char *value) {
        jfieldID testField = env->GetFieldID(classType, fieldName, "Ljava/lang/String;");
        //    jobject str = env->GetObjectField(thisInstance, testField);
        env->SetObjectField(thisInstance, testField, env->NewStringUTF(value));
    }

    void ApiBridge::handleEventTrigger(jobject handlerHook, jobject eventInstanceRaw) {
        eventHookClassType = env->FindClass("net/liz3/skx/include/EventHook");
        jfieldID idField = env->GetFieldID(eventHookClassType, "id", "J");
        long id = env->GetLongField(handlerHook, idField);
        EventPair pair = hooks[id];
       if(!env->IsSameObject(handlerHook, pair.object)){
           std::cout << "returning not same\n";
           return;
       }
        jobject eventInstance = env->NewGlobalRef(eventInstanceRaw);
          executeEventHook(pair.item, pair.event, eventInstance, env);
        env->DeleteGlobalRef(eventInstance);
    }

    jobject ApiBridge::generateEventHook(const char *name, long id) {
        eventHookClassType = env->FindClass("net/liz3/skx/include/EventHook");
        jobject instance = env->AllocObject(eventHookClassType);
        std::cout << (eventHookClassType == nullptr) << "\n";
        jfieldID nameField = env->GetFieldID(eventHookClassType, "name", "Ljava/lang/String;");
        jfieldID idField = env->GetFieldID(eventHookClassType, "id", "J");
        env->SetObjectField(instance, nameField, env->NewStringUTF(name));
        env->SetLongField(instance, idField, id);
        return instance;
    }

    jobject ApiBridge::registerEventHook(const char *evId, long id, jobject pJobject) {
        jobject inst = generateEventHook(evId, id);
        this->classType = env->FindClass("net/liz3/skx/SkxApi");
        jmethodID methodId = env->GetMethodID(classType, "generateListener", "(Lnet/liz3/skx/include/EventHook;)V");
        env->CallObjectMethod(pJobject, methodId, inst);
        return inst;
    }

    bool ApiBridge::loadScript(jobject file, jstring value, jobject pJobject, JNIEnv *env) {
        this->env = env;
        const char *contentP = env->GetStringUTFChars(value, NULL);
        auto *script = skx::Script::parse(contentP);
        AsyncExecutor *executor = new AsyncExecutor();
        executors[script] = executor;
        for (const auto &entry : script->mc_events) {
            long id = hooks.size() + 1;
            std::cout << (entry.first->eventClass) << ":" << id << "\n";
            jobject ref = registerEventHook(entry.first->eventClass.c_str(), id, pJobject);
            hooks[id] = EventPair{
                    .object = env->NewGlobalRef(ref),
                    .item = entry.second,
                    .event = entry.first,
                    .executor = executor
            };
        }
        scripts.push_back(script);
        return true;
    }

    void
    ApiBridge::executeEventHook(CompileItem *target, TriggerEvent *ev, jobject instance, JNIEnv *pEnv) {
         ev->currEventRef = instance;
         ev->env = pEnv;
        Executor::executeStandalone(target);
    }

    bool ApiBridge::runReady() {
        for (auto &entry : executors) {
            for (auto &signal : entry.first->signals) {
                if (signal.first->signalType == TriggerSignal::LOAD) {
                    entry.second->queueFunc([signal]() {
                        Executor::executeStandalone(signal.second);
                    });
                }
            }
        }
        return true;
    }

    bool ApiBridge::runShutdown() {
        for (auto &entry : executors) {
            for (auto &signal : entry.first->signals) {
                if (signal.first->signalType == TriggerSignal::UN_LOAD) {
                    entry.second->queueFunc([signal]() {
                        Executor::executeStandalone(signal.second);
                    });
                }
            }
        }
        return true;
    }


}

// JNI PARTS


JNIEXPORT jboolean JNICALL Java_net_liz3_skx_SkxApi_setupApi
        (JNIEnv *env, jobject thisInstance) {
    if (skx::apiInstance == nullptr) {
        skx::apiInstance = new skx::ApiBridge(env, thisInstance);
    }
    return skx::apiInstance->setup();
}

JNIEXPORT jboolean JNICALL Java_net_liz3_skx_SkxApi_shutdownApi
        (JNIEnv *env, jobject thisInstance) {
    if (skx::apiInstance == nullptr) return false;
    if (skx::apiInstance->shutdown()) {
        delete skx::apiInstance;
        skx::apiInstance = nullptr;
        return true;
    }
    return false;
}

JNIEXPORT void JNICALL Java_net_liz3_skx_SkxApi_handleCommandMessage
        (JNIEnv *env, jobject thisInstance, jobject event) {

}

JNIEXPORT void JNICALL Java_net_liz3_skx_SkxApi_handleEvent
        (JNIEnv *env, jobject thisInstance, jobject hook, jobject event) {
    if (skx::apiInstance == nullptr) return;
    skx::apiInstance->env = env;
    skx::apiInstance->handleEventTrigger(hook, event);
}

JNIEXPORT jboolean JNICALL Java_net_liz3_skx_SkxApi_loadScript
        (JNIEnv *env, jobject thisInstance, jobject file, jstring content) {
    if (skx::apiInstance == nullptr) return false;
    skx::apiInstance->env = env;
    return skx::apiInstance->loadScript(file, content, thisInstance, env);
}


JNIEXPORT jboolean JNICALL Java_net_liz3_skx_SkxApi_fireReady
        (JNIEnv *env, jobject thisInstance) {
    if (skx::apiInstance == nullptr) return false;
    skx::apiInstance->env = env;
    return skx::apiInstance->runReady();
}


JNIEXPORT jboolean JNICALL Java_net_liz3_skx_SkxApi_fireDisable
        (JNIEnv *, jobject) {
    if (skx::apiInstance == nullptr) return false;
    return skx::apiInstance->runShutdown();
}

#endif