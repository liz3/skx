package net.liz3.skx;

import net.liz3.skx.bootstrap.InitResponse;
import net.liz3.skx.include.EventHook;
import org.bukkit.plugin.java.JavaPlugin;

public class Skx extends JavaPlugin {

    private SkxApi apiInstance = null;
    private InitResponse initResponse = null;
    @Override
    public void onLoad() {

        apiInstance = new SkxApi(this);
        apiInstance.setLibPath("/home/liz3/CLionProjects/skx/cmake-build-debug/libskx.so");
        initResponse = apiInstance.initApi();
        if(!initResponse.isSuccess()) {
            System.err.println(initResponse.getMsg());
        } else {
          //  apiInstance.handleEvent(new EventHook("this is the class", null), null);
        }

    }

    @Override
    public void onDisable() {
        if(apiInstance != null) {
            apiInstance.shutdownApi();
        }
    }
}
