//
// Created by liz3 on 11/07/2020.
//
#ifdef SKX_BUILD_API

#ifndef SKX_APIBRIDGE_H
#define SKX_APIBRIDGE_H

#include <jni.h>
#include <string>
#include <vector>
#include <map>
#include "Script.h"
#include "AsyncExecutor.h"

namespace skx {
    struct EventPair {
        jobject object;
        CompileItem* item;
        TriggerEvent* event = nullptr;
        AsyncExecutor* executor;
    };


    class ApiBridge {

    public:
        ApiBridge(JNIEnv_ *pEnv, _jobject *pJobject);
        bool setup();
        bool shutdown();
        void setStringValue(const char* fieldName, const char* value);
        void handleEventTrigger(jobject handlerHook, jobject eventInstance);
        jobject registerEventHook(const char *evId, long id, jobject pJobject);
        jobject generateEventHook(const char* name, long id);
        bool loadScript(jobject file, jstring value, jobject pJobject, JNIEnv *pEnv);
        bool runReady();
        bool runShutdown();
        JNIEnv_ *env;
    private:
        std::map<long, EventPair> hooks;
        void executeEventHook(CompileItem *target, TriggerEvent *ev, jobject instance, const char *const string);
        std::vector<Script*> scripts;
        std::map<Script*, AsyncExecutor*> executors;
        jclass classType;
        jclass eventHookClassType;
        _jobject *thisInstance;
    };
}


#endif //SKX_APIBRIDGE_H

#endif