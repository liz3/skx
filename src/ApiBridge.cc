//
// Created by liz3 on 11/07/2020.
//

#include <iostream>
#include "../include/ApiBridge.h"
#include "../plugin/build/net_liz3_skx_SkxApi.h"

namespace skx {
    skx::ApiBridge * apiInstance = nullptr;

    ApiBridge::ApiBridge(JNIEnv_ *pEnv, _jobject *pJobject) {
        this->env = pEnv;
        this->thisInstance = pJobject;
        this->classType = env->FindClass("net/liz3/skx/SkxApi");
        this->eventHookClassType = env->FindClass("net/liz3/skx/include/EventHook");
    }

    bool ApiBridge::setup() {
      //  setStringValue("apiErrorMessage", "this is a test error");
        registerEventHook();
        return true;
    }

    bool ApiBridge::shutdown() {
        return false;
    }

    void ApiBridge::setStringValue(const char* fieldName, const char* value) {
       jfieldID testField = env->GetFieldID(classType , fieldName, "Ljava/lang/String;");
   //    jobject str = env->GetObjectField(thisInstance, testField);
        env->SetObjectField(thisInstance, testField, env->NewStringUTF(value));
    }

    void ApiBridge::handleEventTrigger(jobject handlerHook, jobject eventInstance) {
        jfieldID nameField = env->GetFieldID(eventHookClassType , "name", "Ljava/lang/String;");
        jstring val = static_cast<jstring>(env->GetObjectField(handlerHook, nameField));
        std::cout << ((bool )env->IsSameObject(handlerHook, hooks[0])) << "\n";
    }

    jobject  ApiBridge::generateEventHook(const char *name) {
        jobject instance = env->AllocObject(eventHookClassType);
        jmethodID constructorId  =env->GetMethodID(eventHookClassType, "<init>", "()V");
        env->CallObjectMethod(instance, constructorId);
        jfieldID nameField = env->GetFieldID(eventHookClassType , "name", "Ljava/lang/String;");
        env->SetObjectField(instance, nameField, env->NewStringUTF(name));
        return instance;
    }

    void ApiBridge::registerEventHook() {
        jobject inst = generateEventHook("onJoin");
        hooks.push_back(inst);
        jmethodID methodId=env->GetMethodID(classType, "generateListener", "(Lnet/liz3/skx/include/EventHook;)V");
        env->CallObjectMethod(thisInstance, methodId, inst);
    }


}

// JNI PARTS


JNIEXPORT jboolean JNICALL Java_net_liz3_skx_SkxApi_setupApi
        (JNIEnv * env, jobject thisInstance) {
    if(skx::apiInstance == nullptr) {
        skx::apiInstance = new skx::ApiBridge(env, thisInstance);
    }
    return skx::apiInstance->setup();
}

JNIEXPORT jboolean JNICALL Java_net_liz3_skx_SkxApi_shutdownApi
        (JNIEnv * env , jobject thisInstance) {
    return skx::apiInstance->shutdown();
}
JNIEXPORT void JNICALL Java_net_liz3_skx_SkxApi_handleCommandMessage
        (JNIEnv * env , jobject thisInstance, jobject event) {

}

JNIEXPORT void JNICALL Java_net_liz3_skx_SkxApi_handleEvent
        (JNIEnv * env, jobject thisInstance, jobject hook, jobject event) {
    skx::apiInstance->handleEventTrigger(hook, event);
}