package net.liz3.skx;

import net.liz3.skx.bootstrap.InitResponse;
import net.liz3.skx.include.EventHook;
import net.liz3.skx.spigot.SkxEventListener;
import org.bukkit.Bukkit;
import org.bukkit.event.EventHandler;
import org.bukkit.event.Listener;
import org.bukkit.event.player.PlayerJoinEvent;
import org.bukkit.plugin.java.JavaPlugin;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;

public class Skx extends JavaPlugin implements Listener {

    private SkxApi apiInstance = null;
    private InitResponse initResponse = null;
    @Override
    public void onEnable() {
        apiInstance = new SkxApi(this);
        apiInstance.setLibPath(System.getenv("SKXLIBPATH"));
        initResponse = apiInstance.initApi();
        if(!initResponse.isSuccess()) {
            System.err.println(initResponse.getMsg());
            return;
        }
        if(!getDataFolder().exists()) {
            getDataFolder().mkdir();
        } else {
            loadScripts(getDataFolder());
            apiInstance.fireReady();
        }

    }
    private void loadScripts(File dataFolder) {
        for(File file : dataFolder.listFiles()) {
            if(file.getName().endsWith(".sk")) {
                try {
                    byte[] data = Files.readAllBytes(file.toPath());
                   if(!apiInstance.loadScript(file, new String(data))) {
                       System.err.println("Failed to load script: " + file.getName());
                   }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    @Override
    public void onDisable() {
        if(apiInstance != null) {
            apiInstance.fireDisable();
            apiInstance.shutdownApi();
        }
    }
}
