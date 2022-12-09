package socket;

import java.io.*;
import java.net.Socket;
import java.net.UnknownHostException;

/**
 * 创建客户端：发送数据+接收数据
 * 写出数据：输出流
 * 读取数据：输入流
 * <p>
 * 输入流与输出流应该独立处理，彼此独立，在不同的线程中
 * 加入名称
 */
public class Client {
    public static void main(String[] args) {
        try {
            //1、创建客户端Socket，指定服务器地址和端口
            Socket socket = new Socket("127.0.0.1", 8888);

            //2、获取输出流，向服务器端发送信息
            OutputStream os = socket.getOutputStream();//字节输出流
            PrintWriter pw = new PrintWriter(os);//将输出流包装成打印流
            pw.write("用户名:bdciwbvduiw密码：udguiwgefuiwge");
            pw.flush();
            socket.shutdownOutput();

            //3、获取输入流，并读取服务器端的响应信息
            InputStream is = socket.getInputStream();
            BufferedReader br = new BufferedReader(new InputStreamReader(is));
            String info = null;
            while ((info = br.readLine()) != null) {
                System.out.println("客户端发送消息" + info);
            }

            //4、关闭资源
            br.close();
            is.close();
            pw.close();
            os.close();
            socket.close();
        } catch (UnknownHostException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
