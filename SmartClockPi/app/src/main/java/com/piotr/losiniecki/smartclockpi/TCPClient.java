package com.piotr.losiniecki.smartclockpi;

import java.io.*;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;

import android.util.Log;
import java.nio.charset.StandardCharsets;

import java.io.StringReader;
import android.util.JsonReader;

public class TCPClient {

    private static final String BROADCAST_ADR = "255.255.255.255";
    private static final int SERVER_PORT = 2016;
    private static final int LISTENER_PORT = 2017;

    private OnServerDisconnected onDisconnected = null;

    private int requestCtr = 0;

    private InetAddress serverAdr = null;
    private Socket connectSocket = null;
    private PrintWriter out = null;
    private BufferedReader in = null;

    private Request getResultRequest() {
        return new Request(requestCtr++, "getLastResult");
    }

    private Request getAlarmTimeRequest() {
        return new Request(requestCtr++, "getAlarmTime");
    }

    private Request getSetAlarmRequest(int hour, int minute) {
        Request req = new Request(requestCtr++, "setClockAlarm");
        req.params.put("hour", Integer.toString(hour));
        req.params.put("minute", Integer.toString(minute));
        return req;
    }

    private Request getLEDStateRequest() {
        return new Request(requestCtr++, "getLEDState");
    }

    private Request getSwitchLEDRequest(int state) {
        Request req = new Request(requestCtr++, "switchLED");
        req.params.put("state", Integer.toString(state));
        return req;

    }

    private Request getDisableAlarmRequest() throws IOException {
        Request req = new Request(requestCtr++, "disableClockAlarm");
        return req;
    }

    private void getServerAdr() {
        try {
            InetAddress broadcastAdr = InetAddress.getByName(BROADCAST_ADR);
            DatagramSocket broadcastSocket = new DatagramSocket();
            String hostRequest = "{\"getClockHost\":false}\n";

            byte[] buf = hostRequest.getBytes();
            DatagramPacket requestPacket = new DatagramPacket(buf, buf.length, broadcastAdr, LISTENER_PORT);
            broadcastSocket.send(requestPacket);

            buf = new byte[128];
            DatagramPacket responsePacket = new DatagramPacket(buf, buf.length);
            broadcastSocket.setSoTimeout(1000);
            broadcastSocket.receive(responsePacket);
            String recvMsg = new String(responsePacket.getData(), StandardCharsets.UTF_8);
            recvMsg = recvMsg.substring(0, responsePacket.getLength());

            boolean responseOk = false;
            JsonReader reader = new JsonReader(new StringReader(recvMsg));
            reader.beginObject();
            responseOk = (reader.nextName().equals("getClockHost") && reader.nextBoolean());
            reader.endObject();

            serverAdr = responseOk ? responsePacket.getAddress() : null;
            serverAdr = responsePacket.getAddress();
        } catch (Exception e) {
            serverAdr = null;
            Log.e("TCPClient", "getServerAdr", e);
        }
    }

    private void connect() {
        try {
            connectSocket = new Socket(serverAdr, SERVER_PORT);
            connectSocket.setTcpNoDelay(true);
            out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(connectSocket.getOutputStream())), true);
            in = new BufferedReader(new InputStreamReader(connectSocket.getInputStream()));
        } catch (Exception e) {
            connectSocket = null;
            out = null;
            in = null;
            Log.e("TCPClient", "connect", e);
        }
    }

    private void disconnect() {
        connectSocket = null;
        out = null;
        in = null;
        onDisconnected.onDisconnected();
    }

    public TCPClient(OnServerDisconnected onServerDisconnected) {
        onDisconnected = onServerDisconnected;
    }

    public void connectToServer() {
        while (serverAdr == null) {
            getServerAdr();
        }
        while (out == null) {
            connect();
        }
    }

    public AlarmTime getAlarmTime() {
        AlarmTime alarmTime = null;
        try {
            if (out != null && !out.checkError()) {
                Request req = getAlarmTimeRequest();
                String msg = req.toJSON();
                if (msg != null)
                    out.println(msg);
                out.flush();

                String respJSON = in.readLine();
                String alarmTimeJSON = in.readLine();

                Response response = Response.getFromJSON(respJSON);
                if(req.id == response.id && response.state == 1) {
                    alarmTime = AlarmTime.getFromJSON(alarmTimeJSON);
                }
            }
        } catch (Exception e) {
            disconnect();
            Log.e("TCPClient", "getAlarmTime", e);
        }
        return alarmTime;
    }

    public Integer getLEDState() {
        Integer alarmState = null;
        try {
            if (out != null && !out.checkError()) {
                Request req = getLEDStateRequest();
                String msg = req.toJSON();
                if (msg != null)
                    out.println(msg);
                out.flush();

                String respJSON = in.readLine();
                String stateJSON = in.readLine();

                Response response = Response.getFromJSON(respJSON);
                if(req.id == response.id && response.state == 1) {
                    alarmState = LEDState.getFromJSON(stateJSON);
                }
            }
        } catch (Exception e) {
            disconnect();
            Log.e("TCPClient", "getLEDState", e);
        }
        return alarmState;
    }

    public void switchLED(int state) {
        boolean responseOk = false;
        try {
            if (out != null && !out.checkError()) {
                Request req = getSwitchLEDRequest(state);
                String msg = req.toJSON();
                if (msg != null)
                    out.println(msg);
                out.flush();

                String respJSON = in.readLine();

                Response response = Response.getFromJSON(respJSON);
                responseOk = (req.id == response.id && response.state == 1);
            }
        } catch (Exception e) {
            Log.e("TCPClient", "switchLED", e);
        }
        if (responseOk == false)
            disconnect();
    }

    public Result getLastResult() {
        Result res = null;
        try {
            if (out != null && !out.checkError()) {
                Request req = getResultRequest();
                String msg = req.toJSON();
                if (msg != null)
                    out.println(msg);
                out.flush();

                String respJSON = in.readLine();
                String resultJSON = in.readLine();

                Response response = Response.getFromJSON(respJSON);
                if(req.id == response.id && response.state == 1) {
                    res = Result.getFromJSON(resultJSON);
                }
            }
        } catch (Exception e) {
            Log.e("TCPClient", "getLastResult", e);
        }
        if (res == null)
            disconnect();
        return res;
    }

    public void setAlarmTime(int hour, int minute) {
        boolean responseOk = false;
        try {
            if (out != null && !out.checkError()) {
                Request req = getSetAlarmRequest(hour, minute);
                String msg = req.toJSON();
                if (msg != null)
                    out.println(msg);
                out.flush();

                String respJSON = in.readLine();

                Response response = Response.getFromJSON(respJSON);
                responseOk = (req.id == response.id && response.state == 1);
            }
        } catch (Exception e) {
            Log.e("TCPClient", "setAlarmTime", e);
        }
        if (responseOk == false)
            disconnect();
    }

    public void disableAlarmTime() {
        boolean responseOk = false;
        try {
            if (out != null && !out.checkError()) {
                Request req = getDisableAlarmRequest();
                String msg = req.toJSON();
                if (msg != null)
                    out.println(msg);
                out.flush();

                String respJSON = in.readLine();

                Response response = Response.getFromJSON(respJSON);
                responseOk = (req.id == response.id && response.state == 1);
            }
        } catch (Exception e) {
            Log.e("TCPClient", "disableAlarmTime", e);
        }
        if (responseOk == false)
            disconnect();
    }

    public interface OnServerDisconnected {
        void onDisconnected();
    }
}