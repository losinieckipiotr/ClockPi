package com.piotr.losiniecki.smartclockpi;

import android.content.Context;
import android.net.wifi.WifiManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.os.AsyncTask;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;


public class MainActivity extends AppCompatActivity {

    private TCPClient mTcpClient;
    private boolean serverConnected = false;

    private ProgressBar connectBar;
    private TextView textBox;
    private Button resultButton;

    private TextView temperature;
    private TextView pressure;
    private TextView time;

    private void serverConnected() {
        serverConnected = true;
        connectBar.setVisibility(View.GONE);
        resultButton.setVisibility(View.VISIBLE);
        textBox.setText(R.string.connected_main_activity);
    }

    private void connectToServer() {
        connectBar.setVisibility(View.VISIBLE);
        resultButton.setVisibility(View.GONE);
        textBox.setText(R.string.connecting_main_activity);
        new ConnectTask().execute();
    }

    private class ConnectTask extends AsyncTask<Void,Void,Void> {
        @Override
        protected Void doInBackground(Void... args) {
            mTcpClient.connectToServer();
            return null;
        }
        @Override
        protected void onPostExecute(Void result) {
            serverConnected();
        }
    }

    private class getResultTask extends AsyncTask<Void, Void, Result> {
        @Override
        protected Result doInBackground(Void... args) {
            return mTcpClient.getLastResult();
        }
        @Override
        protected void onPostExecute(Result result) {
            if (result != null) {
                //temperature.setVisibility(View.VISIBLE);
                temperature.setText("Temperature: " + Double.toString(result.temperature));

                //pressure.setVisibility(View.VISIBLE);
                pressure.setText("Pressure: " + Double.toString(result.pressure));

                //time.setVisibility(View.VISIBLE);
                time.setText("Time: " +
                        Integer.toString(result.timeStamp.hour) +
                        ":" +
                        Integer.toString(result.timeStamp.minute));
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        textBox = (TextView) findViewById(R.id.connectingTxt);
        connectBar = (ProgressBar) findViewById(R.id.connectBar);
        resultButton = (Button) findViewById(R.id.resultButton);

        temperature = (TextView) findViewById(R.id.temp);
        pressure = (TextView) findViewById(R.id.press);
        time = (TextView) findViewById(R.id.time);

        WifiManager wifi = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        wifi.setWifiEnabled(true);

        mTcpClient = new TCPClient(new TCPClient.RpcClient() {
            @Override
            public void onServerDisconnected() {
                serverConnected = false;
                Log.i("MainActivity", "onServerDisconnected");
            }
        });

        resultButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    new getResultTask().execute();
                } catch (Exception e) {
                    Log.e("MainActivity", "resultButton", e);
                }
            }
        });

        connectToServer();
    }
}
