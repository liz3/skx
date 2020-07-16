//
// Created by liz3 on 16/07/2020.
//

#ifndef SKX_JNUUTILS_H
#define SKX_JNUUTILS_H

#include <jni.h>

namespace skx {
    class JniUtils {
    public:
        static jobject getPlayerFromEvent(JNIEnv* env, jobject ref) {
            jfieldID playerId = env->GetFieldID(env->FindClass("org/bukkit/event/player/PlayerEvent"),"player", "Lorg/bukkit/entity/Player;");
            return env->GetObjectField(ref, playerId);
        }
    };
}
#endif //SKX_JNUUTILS_H
