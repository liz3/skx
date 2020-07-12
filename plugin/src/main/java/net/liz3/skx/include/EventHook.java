package net.liz3.skx.include;

import java.time.LocalDateTime;

public class EventHook {
    private String name;
    private long id;
    private Class clazzName;

    public EventHook(String name, Class clazzName) {
        this.name = name;
        this.clazzName = clazzName;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Class getClazzName() {
        return clazzName;
    }

    public void setClazzName(Class clazzName) {
        this.clazzName = clazzName;
    }

    public long getId() {
        return id;
    }

    public void setId(long id) {
        this.id = id;
    }
}
