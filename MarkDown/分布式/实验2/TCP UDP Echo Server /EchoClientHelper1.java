/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package chapter5;

import java.io.*;
import java.net.*;

/**
 *
 * @author qzhang
 */
public class EchoClientHelper1 {
    private MyClientDatagramSocket mySocket;
    private InetAddress serverHost;
    private int serverPort;
    
    EchoClientHelper1(String hostName,String portNum)
            throws  SocketException,UnknownHostException{
        this.serverHost = InetAddress.getByName(hostName);
        this.serverPort = Integer.parseInt(portNum);
        
        this.mySocket = new MyClientDatagramSocket();
    }
    
    public String getEcho(String message)
            throws SocketException,IOException{
        String echo = "";
        mySocket.sendMessage(serverHost, serverPort, message);
        echo = mySocket.receiveMessage();
        return echo;
    }//end getEcho
    
    public void done()throws SocketException{
        mySocket.close();
    }//end done
}//end class
