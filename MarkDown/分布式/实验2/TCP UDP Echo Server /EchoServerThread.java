/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package chapter5;

import java.io.*;

/**
 *
 * @author qzhang
 */
public class EchoServerThread implements Runnable{
    static final String endMessage = ".";
    MyStreamSocket myDataSocket;
    EchoServerThread(MyStreamSocket myDataSocket){
        this.myDataSocket = myDataSocket;
    }//end constructor
    
    public void run(){
        boolean done = false;
        String message;
        try{
            while(!done){
                message = myDataSocket.receiveMessage();
                 System.out.println("message received: " + message);
                    if(message.trim().equals(endMessage)){
                        System.out.println("Session over.");
                        myDataSocket.close();
                        done = true;
                    }//end if
                    else{
                        myDataSocket.sendMessage(message);
                    }//end else
            }//end while !done
        }//end try
        catch(Exception ex){
            System.out.println("Exception caught in thread: " + ex);
        }//end catch
                
    }//end run
}//end class
