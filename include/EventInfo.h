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
                "org.bukkit.event.player.PlayerJoinEvent",
               new const char*[4]{"on join", "on player join", "join", "player join"},
                4
            },
            EventInfo{
                "org.bukkit.event.player.PlayerQuitEvent",
                new const char*[4]{"on leave", "on player leave", "leave", "player leave"},
               4
            },
            EventInfo{
                 "org.bukkit.event.player.AsyncPlayerChatEvent",
               new const char*[6]{"on chat", "on player chat", "chat", "player chat", "on message", "message"},
                 6
            },
            EventInfo{
                "org.bukkit.event.player.PlayerInteractEvent",
                 new const char*[4]{"on click", "on interact", "click", "interact"},
                 4
            },
            EventInfo{
                 "org.bukkit.event.player.PlayerBedEnterEvent",
                   new const char*[4]{"on bed enter", "on enter bed", "bed enter", "enter bed"},
                   4
            },
            EventInfo{
                    "org.bukkit.event.player.PlayerBedLeaveEvent",
                    new const char*[4]{"on bed leave", "on leave bed", "bed leave", "leave bed"},
                   4
            }
    };


}

#endif //SKX_EVENTINFO_H
