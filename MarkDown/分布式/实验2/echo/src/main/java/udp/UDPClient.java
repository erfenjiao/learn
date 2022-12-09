package udp;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class UDPClient {
    public static void main(String[] args) throws IOException {
        //1、定义服务器的地址，端口号，数据
        InetAddress address = InetAddress.getByName("localhost");
        int port = 8800;
        byte[] data = "用户名：admin;密码：12345".getBytes();//将字符串转换为字节数组
        //2、创建DatagramPacket，包含将要发送的信息。
        DatagramPacket packet = new DatagramPacket(data, data.length, address, port);
        //3、创建DatagramSocket，实现数据发送和接收
        DatagramSocket socket = new DatagramSocket();
        //4、向服务器端发送数据报
        socket.send(packet);

        //接收服务器响应数据
        byte[] data2 = new byte[1024];
        DatagramPacket packet2 = new DatagramPacket(data2, data2.length);
        socket.receive(packet2);
        String info = new String(data2, 0, packet2.getLength());
        System.out.println("我是客户端，服务器说：" + info);
        socket.close();
    }
}


