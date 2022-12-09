/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package chapter5;

import java.net.*;

/**
 *
 * @author qzhang
 */
public class DatagramMessage {
    private String message;
    private InetAddress senderAddress;
    private int senderPort;
    public void putVal(String message, InetAddress addr,int port){
        this.message = message;
        this.senderAddress = addr;
        this.senderPort = port;
    }//end putVal
    
    public String getMessage(){
        return this.message;
    }//end getMessage
    
    public InetAddress getAddress(){
        return this.senderAddress;
    }//end getAddress
    
    public int getPort(){
        return this.senderPort;
    }//end getPort
}//end class
