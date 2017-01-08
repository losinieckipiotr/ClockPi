package com.piotr.losiniecki.smartclockpi;

import android.content.Context;
import android.net.wifi.WifiManager;
import android.os.SystemClock;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.os.AsyncTask;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.NumberPicker;

import android.widget.TimePicker;
import android.widget.Toast;

import android.os.Handler;


public class MainActivity extends AppCompatActivity {

    private TCPClient mTcpClient;
    private boolean serverConnected = false;
    private int ledState = 0;

    private TextView infoBox;
    private ProgressBar connectBar;

    private TextView temperature;
    private TextView pressure;
    private TextView time;
    private TextView alarmBox;

    private TimePicker timePicker;
    private Button setAlarmButton;
    private Button disableAlarmButton;
    private Button switchLEDButton;

    Handler mainHandler;

    private void connectingToServer() {
        serverConnected = false;

        infoBox.setVisibility(View.VISIBLE);
        connectBar.setVisibility(View.VISIBLE);

        temperature.setVisibility(View.GONE);
        pressure.setVisibility(View.GONE);
        time.setVisibility(View.GONE);
        alarmBox.setVisibility(View.GONE);

        timePicker.setVisibility(View.GONE);
        setAlarmButton.setVisibility(View.GONE);
        disableAlarmButton.setVisibility(View.GONE);
        switchLEDButton.setVisibility(View.GONE);

        new ConnectTask().execute();
    }

    private void serverConnected() {
        serverConnected = true;

        infoBox.setVisibility(View.GONE);
        connectBar.setVisibility(View.GONE);

        temperature.setVisibility(View.VISIBLE);
        pressure.setVisibility(View.VISIBLE);
        time.setVisibility(View.VISIBLE);
        alarmBox.setVisibility(View.VISIBLE);

        timePicker.setVisibility(View.VISIBLE);
        setAlarmButton.setVisibility(View.VISIBLE);
        disableAlarmButton.setVisibility(View.VISIBLE);
        switchLEDButton.setVisibility(View.VISIBLE);

        updateAll();
    }

    private void updateAlarmTime(AlarmTime alarmTime) {
        if (alarmTime != null) {
            alarmBox.setText(
                    "Alarm is set to: " +
                            Integer.toString(alarmTime.hour) +
                            ":" +
                            Integer.toString(alarmTime.minute));
        }
        else {
            alarmBox.setText("Alarm not set");
        }
    }

    private void updateLEDState(Integer state) {
        if (state != null) {
            if (state == 0) {
                ledState = 0;
                switchLEDButton.setTextColor(getColor(R.color.colorPrimary));
            }
            else if (state == 1) {
                ledState = 1;
                switchLEDButton.setTextColor(getColor(R.color.colorAccent));
            }
        }
    }

    private void updateAll() {
        new GetResultTask().execute();
        new GetAlarmTimeTask().execute();
        new GetLEDStateTask().execute();
    }

    private class ConnectTask extends AsyncTask<Void,Void,Void> {
        @Override
        protected Void doInBackground(Void... args) {
            mTcpClient.connectToServer();
            return null;
        }
        @Override
        protected void onPostExecute(Void result) {
            try {
                serverConnected();
            } catch (Exception e) {
                Log.e("MainActivity", "ConnectTask", e);
            }
        }
    }

    private class GetResultTask extends AsyncTask<Void, Void, Result> {
        @Override
        protected Result doInBackground(Void... args) {
            return mTcpClient.getLastResult();
        }
        @Override
        protected void onPostExecute(Result result) {
            try {
                if (result != null) {
                    temperature.setText("Temperature: " + Double.toString(result.temperature));
                    pressure.setText("Pressure: " + Double.toString(result.pressure));
                    String t = result.timeStamp.year + "-" +
                            result.timeStamp.month + "-" +
                            result.timeStamp.monthDay + " " +
                            result.timeStamp.hour + ":" +
                            result.timeStamp.minute + ":" +
                            result.timeStamp.second;
                    time.setText(t);
                }
            } catch (Exception e) {
                Log.e("MainActivity", "GetResultTask", e);
            }
        }
    }

    private class GetAlarmTimeTask extends AsyncTask<Void, Void, AlarmTime> {
        @Override
        protected AlarmTime doInBackground(Void... args) {
            return mTcpClient.getAlarmTime();
        }
        @Override
        protected void onPostExecute(AlarmTime alarmTime) {
            try {
                updateAlarmTime(alarmTime);
            } catch (Exception e) {
                Log.e("MainActivity", "GetAlarmTimeTask", e);
            }
        }
    }

    private class SetAlarmTask extends AsyncTask<Integer, Void, Void> {
        @Override
        protected Void doInBackground(Integer... args) {
            if (args.length == 2 && args[0] != null && args[1] != null)
                mTcpClient.setAlarmTime(args[0], args[1]);
            return null;
        }
        @Override
        protected void onPostExecute(Void result) {
            try {
                new GetAlarmTimeTask().execute();
            } catch (Exception e) {
                Log.e("MainActivity", "SetAlarmTask", e);
            }
        }
    }

    private class GetLEDStateTask extends AsyncTask<Void, Void, Integer> {
        @Override
        protected Integer doInBackground(Void... args) {
            return mTcpClient.getLEDState();
        }
        @Override
        protected void onPostExecute(Integer state) {
            try {
                updateLEDState(state);
            } catch (Exception e) {
                Log.e("MainActivity", "GetLEDStateTask", e);
            }
        }
    }

    private class SwitchLEDTask extends AsyncTask<Void, Void, Void> {
        @Override
        protected Void doInBackground(Void... args) {
            if (ledState == 0)
                mTcpClient.switchLED(1);
            else if (ledState == 1)
                mTcpClient.switchLED(0);
            return null;
        }
        @Override
        protected void onPostExecute(Void result) {
            try {
                new GetLEDStateTask().execute();
            } catch (Exception e) {
                Log.e("MainActivity", "SwitchLEDTask", e);
            }
        }
    }

    private class DisableAlarmTask extends AsyncTask<Void, Void, Void> {
        @Override
        protected Void doInBackground(Void... args) {
            mTcpClient.disableAlarmTime();
            return null;
        }
        @Override
        protected void onPostExecute(Void result) {
            try {
                new GetAlarmTimeTask().execute();
            } catch (Exception e) {
                Log.e("MainActivity", "DisableAlarmTask", e);
            }
        }
    }

    private class RefreshThread extends Thread {
        public void run() {
            while (true) {
                try {
                    sleep(1000);
                    if (serverConnected) {
                        mainHandler.post(new Runnable() {
                            @Override
                            public void run() {
                                updateAll();
                            }
                        });
                    }
                } catch (Exception e) {
                    Log.e("MainActivity", "RefreshThread", e);
                }
            }
        }
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        infoBox = (TextView) findViewById(R.id.infoBox);
        connectBar = (ProgressBar) findViewById(R.id.connectBar);

        temperature = (TextView) findViewById(R.id.temp);
        pressure = (TextView) findViewById(R.id.press);
        time = (TextView) findViewById(R.id.time);

        alarmBox = (TextView) findViewById(R.id.alarmBox);

        timePicker = (TimePicker) findViewById(R.id.timePicker);

        setAlarmButton = (Button) findViewById(R.id.setAlarmButton);
        disableAlarmButton = (Button) findViewById(R.id.disableAlarmButton);
        switchLEDButton = (Button) findViewById(R.id.switchLEDButton);

        mainHandler = new Handler(getApplicationContext().getMainLooper());

        WifiManager wifi = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        wifi.setWifiEnabled(true);

        mTcpClient = new TCPClient(new TCPClient.OnServerDisconnected() {
            //this callback is not on GUI thread
            @Override
            public void onDisconnected() {
                Runnable onServerDisconnectedHandler = new Runnable() {
                    @Override
                    public void run() {
                        connectingToServer();
                    }
                };
                mainHandler.post(onServerDisconnectedHandler);
                Log.i("MainActivity", "onServerDisconnected");
            }
        });

        setAlarmButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                int hour = timePicker.getHour();
                int minute = timePicker.getMinute();
                new SetAlarmTask().execute(hour, minute);
            }
        });

        disableAlarmButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new DisableAlarmTask().execute();
            }
        });

        switchLEDButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new SwitchLEDTask().execute();
            }
        });

        connectingToServer();
        new RefreshThread().start();
    }
}
