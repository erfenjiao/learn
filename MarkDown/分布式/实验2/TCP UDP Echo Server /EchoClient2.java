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
public class EchoClient2 {
    static final String endMessage = ".";
    public static void main(String[]args){
        InputStreamReader is = new InputStreamReader(System.in);
        BufferedReader br = new BufferedReader(is);
        try{
            System.out.println("Welcome to the Echo client. \n" +
                    "What is the name of the server host?");
            String hostName = br.readLine();
            if(hostName.length()==0)
                hostName = "localhost";
            System.out.println("What is the port number of the server host?");
            String portNum = br.readLine();
            if(portNum.length()==0)
                portNum = "7";
            EchoClientHelper2 helper = 
                    new EchoClientHelper2(hostName,portNum);
            boolean done = false;
            String message,echo;
            while(!done){
                System.out.println("Enter a line to receive an echo back from the server, "
                        +"or a single period to quit.");
                message = br.readLine();
                
                if(message.trim().equals(".")){
                    done = true;
                    helper.done();
                }
                else{
                    echo = helper.getEcho(message);
                    System.out.println(echo);
                }
            }//end while
        }//ech try
        catch(Exception ex){
            ex.printStackTrace();
        }//end catch
    }//end main
}//end class
