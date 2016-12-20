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

import android.widget.Toast;

import android.os.Handler;


public class MainActivity extends AppCompatActivity {

    private TCPClient mTcpClient;
    private boolean serverConnected = false;

    private TextView infoBox;
    private ProgressBar connectBar;

    private Button resultButton;
    private TextView temperature;
    private TextView pressure;
    private TextView time;

    private Button alarmTimeButton;
    private TextView alarmBox;

    private NumberPicker hourPick;
    private NumberPicker minutePick;

    Handler mainHandler;

    private void connectingToServer() {
        serverConnected = false;
        connectBar.setVisibility(View.VISIBLE);
        resultButton.setVisibility(View.GONE);
        infoBox.setText(R.string.connecting_main_activity);
        new ConnectTask().execute();
    }

    private void serverConnected() {
        serverConnected = true;
        connectBar.setVisibility(View.GONE);
        resultButton.setVisibility(View.VISIBLE);
        infoBox.setText(R.string.connected_main_activity);
        new GetAlarmTimeTask();
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
                Log.e("MainActivity", "IsAlarmSetTask", e);
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
                Log.e("MainActivity", "getResultTask", e);
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        infoBox = (TextView) findViewById(R.id.infoBox);
        connectBar = (ProgressBar) findViewById(R.id.connectBar);

        resultButton = (Button) findViewById(R.id.resultButton);
        temperature = (TextView) findViewById(R.id.temp);
        pressure = (TextView) findViewById(R.id.press);
        time = (TextView) findViewById(R.id.time);

        alarmTimeButton = (Button) findViewById(R.id.alarmTimeButton);
        alarmBox = (TextView) findViewById(R.id.alarmBox);

        hourPick = (NumberPicker) findViewById(R.id.hourPicker);
        hourPick.setMinValue(0);
        hourPick.setMaxValue(24);
        minutePick = (NumberPicker) findViewById(R.id.minutePicker);
        minutePick.setMinValue(0);
        minutePick.setMaxValue(60);

        mainHandler = new Handler(getApplicationContext().getMainLooper());

        WifiManager wifi = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        wifi.setWifiEnabled(true);

        mTcpClient = new TCPClient(new TCPClient.RpcClient() {
            //this callback is not on GUI thread
            @Override
            public void onServerDisconnected() {
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

        resultButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    new GetResultTask().execute();
                } catch (Exception e) {
                    Log.e("MainActivity", "resultButton", e);
                }
            }
        });

        alarmTimeButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    new GetAlarmTimeTask().execute();
                } catch (Exception e) {
                    Log.e("MainActivity", "alarmTimeButton", e);
                }
            }
        });

        connectingToServer();
    }
}
