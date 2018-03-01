package com.mindx.doubleprocess;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.Looper;
import android.os.Process;
import android.support.annotation.Nullable;
import android.util.Log;
import android.widget.Toast;

import java.sql.Time;
import java.util.Timer;
import java.util.TimerTask;


/*
* 被守护线程
* */
public class ProcessService extends Service {
    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        DoubleProcess process=new DoubleProcess();
        process.createDoubleProcess(String.valueOf(Process.myPid()));
        process.connectMonitor();
        Timer time=new Timer();
        time.schedule(new TimerTask() {
            @Override
            public void run() {
                Log.e("123","1234123412341234");
            }
        },0,1000*3);
    }
}
