//
// Created by liz3 on 11/07/2020.
//

#ifndef SKX_APIBRIDGE_H
#define SKX_APIBRIDGE_H

#include <jni.h>
#include <string>
#include <vector>

namespace skx {
    class ApiBridge {

    public:
        ApiBridge(JNIEnv_ *pEnv, _jobject *pJobject);
        bool setup();
        bool shutdown();
        void setStringValue(const char* fieldName, const char* value);
        void handleEventTrigger(jobject handlerHook, jobject eventInstance);

        void registerEventHook();
        jobject generateEventHook(const char* name);

    private:
        std::vector<jobject> hooks;
        jclass classType;
        jclass eventHookClassType;
        JNIEnv_ *env;
        _jobject *thisInstance;
    };
}


#endif //SKX_APIBRIDGE_H
