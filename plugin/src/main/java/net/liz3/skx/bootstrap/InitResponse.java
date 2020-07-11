package net.liz3.skx.bootstrap;

public class InitResponse {
    private boolean success;
    private  String msg;

    public InitResponse(boolean success, String msg) {
        this.success = success;
        this.msg = msg;
    }

    public boolean isSuccess() {
        return success;
    }

    public String getMsg() {
        return msg;
    }
}
