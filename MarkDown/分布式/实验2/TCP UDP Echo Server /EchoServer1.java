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
public class EchoServer1 {
    public static void main(String[]args){
        int serverPort = 7;
        if(args.length == 1)
            serverPort = Integer.parseInt(args[0]);
        try{
            MyServerDatagramSocket mySocket =
                    new MyServerDatagramSocket(serverPort);
            System.out.println("Echo Sever ready.");
            
            while(true){
                DatagramMessage request = 
                        mySocket.receiveMessageAndSender();
                System.out.println("Reeuest received");
                String message = request.getMessage();
                System.out.println("message received: " + message);
                
                mySocket.sendMessage(request.getAddress(), request.getPort(), message);
                
                Thread.sleep(60000);
            }//end while
        }//end try
        catch(Exception ex){
            ex.printStackTrace();
        }
    }//end main
}//end class
