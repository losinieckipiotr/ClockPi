package com.piotr.losiniecki.smartclockpi;

import java.io.*;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;

import android.util.Log;

import java.nio.charset.StandardCharsets;

import android.util.JsonWriter;
import android.util.JsonReader;
import java.io.StringWriter;
import java.io.StringReader;

public class TCPClient {

    private String serverMessage;
    public static final String BROADCAST_ADDR = "255.255.255.255";
    public static final int SERVER_PORT = 2016;
    public static final int LISTENER_PORT = 2017;
    private OnMessageReceived mMessageListener = null;
    private boolean mRun = false;
    private int requestCtr = 0;

    PrintWriter out;
    BufferedReader in;

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

    private String getRequest(String name) throws IOException {
        String request = null;
        switch (name) {
            case "result":
                request = getResultRequest();
                break;
            case "history":
                request = getHistoryRequest();
                break;
            case "time":
                request = getAlarmTimeRequest();
                break;
            case "alarm":
                request = getSetAlarmRequest();
                break;
            case "disable":
                request = getDisableAlarmRequest();
                break;
            default:
                break;
        }
        return request;
    }

    /**
     *  Constructor of the class. OnMessagedReceived listens for the messages received from server
     */
    public TCPClient(OnMessageReceived listener) {
        mMessageListener = listener;
    }

    /**
     * Sends the message entered by client to the server
     * @param message text entered by client
     */
    public void sendMessage(String message){
        try {
            if (out != null && !out.checkError()) {
                String msg = getRequest(message);
                if (msg != null)
                    out.println(msg);
                out.flush();
            }
        } catch (Exception e) {
            Log.e("TCP", "C: Error", e);
        }
    }

    public void stopClient(){
        mRun = false;
    }

    public void run() {

        mRun = true;

        try {
            InetAddress broadcastAddr = InetAddress.getByName(BROADCAST_ADDR);
            DatagramSocket broadcastSocket = new DatagramSocket();
            String hostRequest = "{\"getClockHost\":false}\n";

            byte[] buf = hostRequest.getBytes();
            DatagramPacket requestPacket = new DatagramPacket(buf, buf.length, broadcastAddr, LISTENER_PORT);
            broadcastSocket.send(requestPacket);

            buf = new byte[128];
            DatagramPacket responsePacket = new DatagramPacket(buf, buf.length);
            broadcastSocket.receive(responsePacket);
            String recvMsg = new String(responsePacket.getData(), StandardCharsets.UTF_8);
            recvMsg = recvMsg.substring(0, responsePacket.getLength());

            try {
                Boolean responseOk = false;
                JsonReader reader = new JsonReader(new StringReader(recvMsg));
                reader.beginObject();
                responseOk = (reader.nextName().equals("getClockHost") && reader.nextBoolean());
                reader.endObject();
                if(!responseOk)
                    return;
            } catch (Exception e) {
                Log.e("UDP", "Read error", e);
            }

            InetAddress serverAddr = responsePacket.getAddress();
            Socket socket = new Socket(serverAddr, SERVER_PORT);
            socket.setTcpNoDelay(true);

            try {
                //send the message to the server
                out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(socket.getOutputStream())), true);

                //receive the message which the server sends back
                in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

                out.println(getResultRequest());

                while (mRun) {
                    serverMessage = in.readLine();

                    if (serverMessage != null && mMessageListener != null) {
                        //call the method messageReceived from MyActivity class
                        mMessageListener.messageReceived(serverMessage);
                    }
                    serverMessage = null;
                }
                socket.close();

            } catch (Exception e) {
                Log.e("TCP", "S: Error", e);
            } finally {

                stopClient();
            }
        } catch (Exception e) {
            Log.e("TCP", "C: Error", e);
        }
    }

    //Declare the interface. The method messageReceived(String message) will must be implemented in the MyActivity
    //class at on asynckTask doInBackground
    public interface OnMessageReceived {
        public void messageReceived(String message);
    }
}