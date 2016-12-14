package com.piotr.losiniecki.smartclockpi;

import java.io.*;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;

import android.util.Log;

import java.nio.charset.StandardCharsets;

public class TCPClient {

    private String serverMessage;
    public static final String BROADCAST_ADDR = "255.255.255.255";
    public static final int SERVER_PORT = 2016;
    public static final int LISTENER_PORT = 2017;
    private OnMessageReceived mMessageListener = null;
    private boolean mRun = false;

    PrintWriter out;
    BufferedReader in;

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
        if (out != null && !out.checkError()) {
            out.println(message);
            out.flush();
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
            String recvMsg = new String(responsePacket.getData(), StandardCharsets.UTF_8).substring(0, responsePacket.getLength());

            if(!recvMsg.equals("{\"getClockHost\":true}\n"))
                return;

            InetAddress serverAddr = responsePacket.getAddress();
            Socket socket = new Socket(serverAddr, SERVER_PORT);
            socket.setTcpNoDelay(true);

            try {

                //send the message to the server
                out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(socket.getOutputStream())), true);

                //receive the message which the server sends back
                in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

                out.println("{\"request\":{\"id\":\"0\",\"name\":\"getLastResult\",\"params\":\"\"}}");

                while (mRun) {
                    serverMessage = in.readLine();

                    if (serverMessage != null && mMessageListener != null) {
                        //call the method messageReceived from MyActivity class
                        mMessageListener.messageReceived(serverMessage);
                    }
                    serverMessage = null;
                }

                Log.e("RESPONSE FROM SERVER", "S: Received Message: '" + serverMessage + "'");

            } catch (Exception e) {

                Log.e("TCP", "S: Error", e);

            } finally {
                socket.close();
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