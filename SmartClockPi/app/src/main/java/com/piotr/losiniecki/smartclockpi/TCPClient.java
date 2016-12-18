package com.piotr.losiniecki.smartclockpi;

import java.io.*;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;

import android.util.Log;
import java.nio.charset.StandardCharsets;

import android.util.JsonWriter;
import java.io.StringWriter;

import java.io.StringReader;
import android.util.JsonReader;

public class TCPClient {

    public static final String BROADCAST_ADR = "255.255.255.255";
    public static final int SERVER_PORT = 2016;
    public static final int LISTENER_PORT = 2017;

    RpcClient rpc = null;

    private int requestCtr = 0;

    private InetAddress serverAdr = null;

    private Socket connectSocket = null;
    private PrintWriter out = null;
    private BufferedReader in = null;

    private void beginRequest(JsonWriter writer) throws IOException {
        writer.beginObject();
        writer.name("request");
        writer.beginObject();
        writer.name("id").value(requestCtr++);
    }

    private void endRequest(JsonWriter writer) throws IOException {
        writer.endObject();
        writer.endObject();
        writer.close();
    }

    private String getResultRequest() throws IOException {
        StringWriter str = new StringWriter();
        JsonWriter writer = new JsonWriter(str);

        beginRequest(writer);

        writer.name("name").value("getLastResult");
        writer.name("params").value("");

        endRequest(writer);

        return str.toString();
    }

    private String getHistoryRequest() throws IOException {
        StringWriter str = new StringWriter();
        JsonWriter writer = new JsonWriter(str);

        beginRequest(writer);

        writer.name("name").value("getResultsHistory");
        writer.name("params").value("");

        endRequest(writer);

        return str.toString();
    }

    private String getAlarmTimeRequest() throws IOException {
        StringWriter str = new StringWriter();
        JsonWriter writer = new JsonWriter(str);

        beginRequest(writer);

        writer.name("name").value("getAlarmTime");
        writer.name("params").value("");

        endRequest(writer);

        return str.toString();
    }

    private String getSetAlarmRequest() throws IOException {
        StringWriter str = new StringWriter();
        JsonWriter writer = new JsonWriter(str);

        beginRequest(writer);

        writer.name("name").value("setClockAlarm");
        writer.name("params");
        writer.beginObject();
        writer.name("hour").value(16);
        writer.name("minute").value(0);
        writer.endObject();

        endRequest(writer);

        return str.toString();
    }

    private String getDisableAlarmRequest() throws IOException {
        StringWriter str = new StringWriter();
        JsonWriter writer = new JsonWriter(str);

        beginRequest(writer);

        writer.name("name").value("disableClockAlarm");
        writer.name("params").value("");

        endRequest(writer);

        return str.toString();
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
        try {
            connectSocket = null;
            out = null;
            in = null;
            rpc.onServerDisconnected();
        } catch (Exception e) {
            Log.e("TCPClient", "disconnect", e);
        }
    }

    public TCPClient(RpcClient rpcClient) {
        rpc = rpcClient;
    }

    public void stopClient() {
        disconnect();
    }

    public void connectToServer() {
        while (serverAdr == null) {
            getServerAdr();
        }
        while (out == null) {
            connect();
        }
    }

    public Result getLastResult() {
        try {
            if (out != null && !out.checkError()) {
                String msg = getResultRequest();
                if (msg != null)
                    out.println(msg);
                out.flush();

                String respJSON = in.readLine();
                String resultJSON = in.readLine();

                Response response = Response.getFromJSON(respJSON);
                Result lastResult = Result.getFromJSON(resultJSON);
                return lastResult;
            }
        } catch (Exception e) {
            disconnect();
            Log.e("TCPClient", "sendMessage", e);
        }
        return null;
    }

    public interface RpcClient {
        void onServerDisconnected();
    }
}