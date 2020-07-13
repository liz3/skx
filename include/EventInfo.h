//
// Created by liz3 on 13/07/2020.
//

#ifndef SKX_EVENTINFO_H
#define SKX_EVENTINFO_H

#include <cstdlib>

namespace skx {
    struct EventInfo {
        const char* javaClass;
        const char** matches;
        size_t len;
    };

    EventInfo eventMap[] = {
            EventInfo{
                .javaClass = "org.bukkit.event.player.PlayerJoinEvent",
                .matches = new const char*[2]{"on join", "on player join"},
                .len = 2
            },
            EventInfo{
                    .javaClass = "org.bukkit.event.player.PlayerBedEnterEvent",
                    .matches = new const char*[2]{"on bed enter", "on enter bed"},
                    .len = 2
            }
    };


}

#endif //SKX_EVENTINFO_H
