package com.mindx.doubleprocess;

/**
 * Created by Administrator on 2018/2/27.
 */

public class DoubleProcess {
    static {
        System.loadLibrary("native-lib");
    }
    public native void createDoubleProcess(String pid );
    public native void  connectMonitor();
}
