#include<cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <signal.h>

#define BUFFSIZE 2048


class ServerSocket {
   public:
    ServerSocket() {
        sockfd = -1;
        connfd = -1;
    }
    ~ServerSocket() {
        if (1){
            ;
        }
    }
    bool DoInit() {
        /**
         * 创建套接字 
         */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            printf("Create socket error(%d):%s\n", errno, strerror(errno));
            return false;
        }
        return true;
    }
     /**
     * 绑定ip和端口 
     */
    bool DoBind(const char* ip, short port = 6000) {
        struct sockaddr_in servaddr; // 用于存放ip和端口的结构
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(port);
        if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
            printf("Bind error(%d): %s\n", errno, strerror(errno));
            return false;
        }
        return true;
    }
    /**
     * 监听 
     */
    bool DoListen() {
        if(listen(sockfd, 1024) == -1) {
            printf("Listen error(%d): %s\n", errno, strerror(errno));
            return false;
        }
        return true;
    }
    /**
     * 循环等待客户端到来 
     */
    bool DoAccept() {
        while(true) {
            //signal(SIGINT, stopServerRunning);    // 这句用于在输入Ctrl+C的时候关闭服务器
            connfd = accept(sockfd, NULL, NULL);
            if(connfd == -1) {
                printf("Accept error(%d): %s\n", errno, strerror(errno));
                return false;
            }
            bzero(buff, BUFFSIZE);
            // 对应伪代码中的recv(connfd, buff);
            recv(connfd, buff, BUFFSIZE - 1, 0);
            // END
            printf("Recv: %s\n", buff);
            // 对应伪代码中的send(connfd, buff);
            send(connfd, buff, strlen(buff), 0);
            // END
            // 对应伪代码中的close(connfd);
            close(connfd);
        }
        return false;
    }
    private:
        int sockfd, connfd;
        char buff[BUFFSIZE];
};


int main() {
    ServerSocket serverSocket;
    if(!serverSocket.DoInit())
        return false;
    if(!serverSocket.DoBind("127.0.0.1", 6000))
        return false;
    if(!serverSocket.DoListen()) 
        return false;
    if(!serverSocket.DoAccept())
        return false;
    return 0;
}
