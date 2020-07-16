//
// Created by liz3 on 16/07/2020.
//

#include "../../include/api/McEventValues.h"
#include "../../include/types/TString.h"
#include <jni.h>

skx::OperatorPart *skx::PlayerName::execute(skx::Context *target) {
    if(ref == nullptr) return nullptr;
    JNIEnv* env = ref->env;
    jfieldID playerId = env->GetFieldID(env->FindClass("org/bukkit/event/player/PlayerEvent"),"player", "Lorg/bukkit/entity/Player;");
    jobject player = env->GetObjectField(ref->currEventRef, playerId);
    jmethodID nameMethod = env->GetMethodID(env->FindClass("org/bukkit/entity/Player"), "getName", "()Ljava/lang/String;");
    jstring entry = static_cast<jstring>(env->CallObjectMethod(player, nameMethod));
    const char* val = env->GetStringUTFChars(entry, NULL);
    return new OperatorPart(LITERAL, STRING, new TString(std::string(val)), false, true);
}

skx::PlayerName::~PlayerName() {

}
