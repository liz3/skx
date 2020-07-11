////
//// Created by liz3 on 11/07/2020.
////
//
//#include "../include/jni/net_liz3_skx_Bridge.h"
//#include <iostream>
//
//JNIEXPORT void JNICALL Java_net_liz3_skx_Bridge_parseScript
//(JNIEnv* env, jobject obj, jstring str) {
//    const char* nameCharPointer = env->GetStringUTFChars(str, NULL);
//
//}
//
//JNIEXPORT void JNICALL Java_net_liz3_skx_Bridge_invokeNative
//        (JNIEnv * env, jobject thisInstance, jobject event) {
//    // Find the id of the Java method to be called
//    jclass userDataClass=env->GetObjectClass(event);
//    jmethodID methodId=env->GetMethodID(userDataClass, "callThis", "(Lnet/liz3/skx/Event;)V");
//
//    jclass eventClass = env->FindClass("net/liz3/skx/Event");
//    jobject eventInstance = env->AllocObject(eventClass);
//    jfieldID testField = env->GetFieldID(eventClass , "test", "Ljava/lang/String;");
////    env->SetObjectField(eventInstance, testField, env->NewStringUTF("This is the Value set from c++"));
//    jstring str = static_cast<jstring>(env->GetObjectField(event, testField));
//    std::cout << (env->GetStringUTFChars(str, NULL)) << "\n";
//   // env->CallObjectMethod(event, methodId, eventInstance);
//
//}