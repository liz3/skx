package net.liz3.skx;

import net.liz3.skx.bootstrap.InitResponse;
import net.liz3.skx.include.EventHook;
import org.bukkit.event.Event;
import org.bukkit.event.player.AsyncPlayerChatEvent;

public class SkxApi {

    private String apiErrorMessage; //this gets set by the native in case anything goes wrong
    private final Skx skx;
    private String libPath;
    private boolean loaded = false;
    
    public SkxApi(Skx skx) {
        this.skx = skx;
    }

    public InitResponse initApi() {
        if(loaded) {
            return null;
        }
        //Load native Library
        try {
            System.load(libPath);
        } catch(Exception e) {
            return new InitResponse(false, e.getLocalizedMessage());
        }


        if(!setupApi()) {
            return new InitResponse(false, apiErrorMessage);
        }
        loaded = true;
        return new InitResponse(true, "SuccessFully Loaded");
    }


    public native boolean setupApi();
    public native boolean shutdownApi();

    public void generateListener(EventHook hook) {
        System.out.println("CALLLED " + hook.getName());
        handleEvent(hook, null);
    }

    //native events(only command ig)
    public native void handleCommandMessage(AsyncPlayerChatEvent asyncPlayerChatEvent);

    public native void handleEvent(EventHook hook, Event event);


    public String getLibPath() {
        return libPath;
    }

    public void setLibPath(String libPath) {
        this.libPath = libPath;
    }
}
