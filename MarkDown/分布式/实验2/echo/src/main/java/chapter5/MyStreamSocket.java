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
public class MyStreamSocket extends Socket{
    private  Socket socket;
    private BufferedReader input;
    private PrintWriter output;
    
    MyStreamSocket(String acceptorHost, int acceptorPort)throws SocketException,IOException{
        socket = new Socket(acceptorHost, acceptorPort);
        setStreams();
    }
    MyStreamSocket(Socket socket)throws IOException{
        this.socket = socket;
        setStreams();
    }
    
    private void setStreams()throws IOException{
        InputStream inStream = socket.getInputStream();
        input = 
                new BufferedReader(new InputStreamReader(inStream));
        OutputStream outStream = socket.getOutputStream();
        output = 
                new PrintWriter(new OutputStreamWriter(outStream));
    }
    
    public void sendMessage(String message)
            throws IOException{
        output.println(message);
        output.flush();
    }
    
    public String receiveMessage()
            throws IOException{
        String message = input.readLine();
        return message;
    }
}
