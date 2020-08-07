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

skx::InteractItem::~InteractItem() {

}

skx::OperatorPart *skx::InteractItem::execute(skx::Context *target) {
    if(ref == nullptr) return nullptr;
    JNIEnv* env = ref->env;
    jfieldID itemStackField = env->GetFieldID(env->FindClass("org/bukkit/event/player/PlayerInteractEvent"),"item", "Lorg/bukkit/inventory/ItemStack;");
    jobject itemStack = env->GetObjectField(ref->currEventRef, itemStackField);
    jmethodID methodId = env->GetMethodID(env->FindClass("org/bukkit/inventory/ItemStack"), "getType", "()Lorg/bukkit/Material;");
    jobject materialInstance = env->CallObjectMethod(itemStack, methodId);
    jmethodID toString = env->GetMethodID(env->FindClass("org/bukkit/Material"), "name", "()Ljava/lang/String;");
    jstring resultJString = static_cast<jstring>(env->CallObjectMethod(materialInstance, toString));
    if(resultJString == nullptr) {
        return new OperatorPart(LITERAL, STRING, new TString(""), false, true);

    }
    std::string strVal = std::string(env->GetStringUTFChars(resultJString, NULL));
    std::transform(strVal.begin(), strVal.end(), strVal.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return new OperatorPart(LITERAL, STRING, new TString(strVal), false, true);

}

skx::TargetedItem::~TargetedItem() {

}

skx::OperatorPart *skx::TargetedItem::execute(skx::Context *target) {
    if(ref == nullptr) return nullptr;
    JNIEnv* env = ref->env;
    jfieldID itemStackField = env->GetFieldID(env->FindClass("org/bukkit/event/player/PlayerInteractEvent"),"blockClicked", "Lorg/bukkit/block/Block;");
    jobject itemStack = env->GetObjectField(ref->currEventRef, itemStackField);
    jmethodID methodId = env->GetMethodID(env->FindClass("org/bukkit/block/Block"), "getType", "()Lorg/bukkit/Material;");
    jobject materialInstance = env->CallObjectMethod(itemStack, methodId);
    jmethodID toString = env->GetMethodID(env->FindClass("org/bukkit/Material"), "name", "()Ljava/lang/String;");
    jstring resultJString = static_cast<jstring>(env->CallObjectMethod(materialInstance, toString));
    if(resultJString == nullptr) {
        return new OperatorPart(LITERAL, STRING, new TString(""), false, true);

    }
    std::string strVal = std::string(env->GetStringUTFChars(resultJString, NULL));
    std::transform(strVal.begin(), strVal.end(), strVal.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return new OperatorPart(LITERAL, STRING, new TString(strVal), false, true);

}
