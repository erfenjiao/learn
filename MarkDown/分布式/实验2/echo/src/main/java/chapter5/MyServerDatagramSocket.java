/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package chapter5;

import java.net.*;
import java.io.*;

/**
 *
 * @author qzhang
 */
public class MyServerDatagramSocket extends DatagramSocket{
    static final int MAX_LEN = 100;
    MyServerDatagramSocket(int portNo)throws SocketException{
        super(portNo);
    }
    public void sendMessage(InetAddress receiverHost,int receiverPort,String message)throws IOException{
        byte[] sendBuffer = message.getBytes();
        DatagramPacket datagram =
                new DatagramPacket(sendBuffer,sendBuffer.length,receiverHost,receiverPort);
        this.send(datagram);
    }//end sendMessage
    
    public String receiveMessage() throws IOException{
        byte[] receiveBuffer = new byte[MAX_LEN];
        DatagramPacket datagram = 
                new DatagramPacket(receiveBuffer,MAX_LEN);
        this.receive(datagram);
        String message = new String(receiveBuffer);
        return message;
    }//end receiveMessage
    
    public DatagramMessage receiveMessageAndSender()throws IOException{
        byte[] receiveBuffer = new byte[MAX_LEN];
        DatagramPacket datagram = 
                new DatagramPacket(receiveBuffer,MAX_LEN);
        this.receive(datagram);
        
        DatagramMessage returnVal = new DatagramMessage();
        returnVal.putVal(new String(receiveBuffer),datagram.getAddress(),datagram.getPort());
        return returnVal;
    }//end receiveMessageAndSender
    
}//end class
