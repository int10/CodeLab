package com.int10.myapplication;

import android.app.Notification;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by int10 on 2017/9/19.
 */

public class MyService extends Service {
	private final String ACTION_TOAST = "action_toast";
	private String data = "默认消息";
	private boolean serviceRunning = false;

	// 必须实现的方法，用于返回Binder对象
	@Override
	public IBinder onBind(Intent intent) {
		System.out.println("--onBind()--");
		return new MyBinder();
	}

	public class MyBinder extends Binder {
		MyService getService() {
			return MyService.this;
		}

		public void setData(String data) {
			MyService.this.data = data;
		}
	}

	// 创建Service时调用该方法，只调用一次
	@Override
	public void onCreate() {
		super.onCreate();
		System.out.println("--onCreate()--");
		Log.e("int10", "creat");
		serviceRunning = true;
		registerReceiver(receiver, new IntentFilter(ACTION_TOAST));
		startForeground(1024, new Notification());

		Timer timer = new Timer();
		TimerTask timerTask = new TimerTask() {
			@Override
			public void run() {
				mHandler.sendEmptyMessage(HANDLER_SHOW_MSG);
			}
		};
		timer.scheduleAtFixedRate(timerTask, 0, 2000L);
	}


	final private int HANDLER_SHOW_MSG = 1000;
	Handler mHandler = new Handler() {
		public void handleMessage(android.os.Message msg) {
			switch (msg.what) {
				case HANDLER_SHOW_MSG:
//                    NotificationManager manager;
//                    manager=(NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
//                    Notification.Builder builder=new Notification.Builder(MyAccessibilityService.this);
//                    builder.setWhen(System.currentTimeMillis());//设置通知时间
//                    builder.setDefaults(Notification.DEFAULT_VIBRATE);//设置震动
//                    Notification notification=builder.build();//4.1以上，以下要用getNotification()
//                    manager.notify(0, notification);
					//m_phonemobile.ShowToast();
                    SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
                    Toast.makeText(MyService.this, "DSFASFADSFDASASADFASD" , Toast.LENGTH_SHORT).show();
					System.out.println("asdfdasfasdfdas");
//					sendBroadcast(new Intent(ACTION_TOAST));
//					mHandler.sendEmptyMessageDelayed(HANDLER_SHOW_MSG, 3000L);
					break;
				default:
					break;
			}
		};
	};

	// 每次启动Servcie时都会调用该方法
	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		System.out.println("--onStartCommand()--");
		data = intent.getStringExtra("data");
		return super.onStartCommand(intent, flags, startId);
	}

	// 解绑Servcie调用该方法
	@Override
	public boolean onUnbind(Intent intent) {
		System.out.println("--onUnbind()--");
		return super.onUnbind(intent);
	}

	// 退出或者销毁时调用该方法
	@Override
	public void onDestroy() {
		serviceRunning = false;
		System.out.println("--onDestroy()--");
		if (null != receiver) {
			unregisterReceiver(receiver);
		}
		super.onDestroy();
	}

	DataCallback dataCallback = null;

	public DataCallback getDataCallback() {
		return dataCallback;
	}

	public void setDataCallback(DataCallback dataCallback) {
		this.dataCallback = dataCallback;
	}

	// 通过回调机制，将Service内部的变化传递到外部
	public interface DataCallback {
		void dataChanged(String str);
	}

	public BroadcastReceiver receiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
			Toast.makeText(MyService.this, df.format(new Date()) , Toast.LENGTH_SHORT).show();
		}
	};

}
