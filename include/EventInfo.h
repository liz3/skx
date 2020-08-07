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
                .matches = new const char*[4]{"on join", "on player join", "join", "player join"},
                .len = 4
            },
            EventInfo{
                .javaClass = "org.bukkit.event.player.PlayerQuitEvent",
                .matches = new const char*[4]{"on leave", "on player leave", "leave", "player leave"},
                .len = 4
            },
            EventInfo{
                .javaClass = "org.bukkit.event.player.AsyncPlayerChatEvent",
                .matches = new const char*[6]{"on chat", "on player chat", "chat", "player chat", "on message", "message"},
                .len = 6
            },
            EventInfo{
                .javaClass = "org.bukkit.event.player.PlayerInteractEvent",
                .matches = new const char*[4]{"on click", "on interact", "click", "interact"},
                .len = 4
            },
            EventInfo{
                    .javaClass = "org.bukkit.event.player.PlayerBedEnterEvent",
                    .matches = new const char*[4]{"on bed enter", "on enter bed", "bed enter", "enter bed"},
                    .len = 4
            },
            EventInfo{
                    .javaClass = "org.bukkit.event.player.PlayerBedLeaveEvent",
                    .matches = new const char*[4]{"on bed leave", "on leave bed", "bed leave", "leave bed"},
                    .len = 4
            }
    };


}

#endif //SKX_EVENTINFO_H
