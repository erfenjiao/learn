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
public class EchoClientHelper2 {
    static final String endMessage = ".";
    private MyStreamSocket mySocket;
    private InetAddress serverHost;
    private int serverPort;
    
    EchoClientHelper2(String hostName,String portNum)
            throws  SocketException,UnknownHostException, IOException{
        this.serverHost = InetAddress.getByName(hostName);
        this.serverPort = Integer.parseInt(portNum);
        
        this.mySocket = new MyStreamSocket(hostName,this.serverPort);
        System.out.println("Connection request made");
    }
    
    public String getEcho(String message)
            throws SocketException,IOException{
        String echo = "";
        mySocket.sendMessage(message);
        echo = mySocket.receiveMessage();
        return echo;
    }//end getEcho
    
    public void done()throws SocketException,IOException{
        mySocket.sendMessage(endMessage);
        mySocket.close();
    }//end done
}//end class
