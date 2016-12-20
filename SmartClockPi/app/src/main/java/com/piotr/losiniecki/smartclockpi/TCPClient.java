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

    private RpcClient rpc = null;

    private int requestCtr = 0;

    private InetAddress serverAdr = null;
    private Socket connectSocket = null;
    private PrintWriter out = null;
    private BufferedReader in = null;

    private Request getResultRequest() {
        return new Request(requestCtr++, "getLastResult");
    }

    private String getHistoryRequest() {
        Request req = new Request(requestCtr++, "getResultsHistory");
        return req.toJSON();
    }

    private Request getAlarmTimeRequest() {
        return new Request(requestCtr++, "getAlarmTime");
    }

    private String getSetAlarmRequest() {
        Request req = new Request(requestCtr++, "setClockAlarm");
        req.params.put("hour", "16");
        req.params.put("minute", "0");
        return req.toJSON();
    }

    private String getDisableAlarmRequest() throws IOException {
        Request req = new Request(requestCtr++, "disableClockAlarm");
        return req.toJSON();
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
        rpc.onServerDisconnected();
    }

    public TCPClient(RpcClient rpcClient) {
        rpc = rpcClient;
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
            Log.e("TCPClient", "sendMessage", e);
        }
        return alarmTime;
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
            Log.e("TCPClient", "sendMessage", e);
        }
        if (res == null)
            disconnect();
        return res;
    }

    public interface RpcClient {
        void onServerDisconnected();
    }
}