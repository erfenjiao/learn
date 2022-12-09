/**
 *  写一个c++程序，用该程序实现一个简单的服务，在客户端连接上来时，给客户端发一条“Hello World”消息后关闭连接，
 *  不用保证客户端一定能收到
 *  知识点：网络编程
 *  基本流程：
 *  （1）创建Socket套接字
 *  （2）绑定ip地址和端口
 *  （3）启动监听，循环等待客户端到来，在客户端连接成功后，给客户端发一条“Hello World”消息后关闭连接
 * 
 *   ------------------------
 * 
 *   if(操作不成功) {
 *       delete[] p;
 *       p = NULL;
 *       return ;
 *    }
 *    这种类型的操作可以概括为：“先分配资源，再进行相关操作，在任意中间步骤出错都对相应的资源进行回收”
 *    这样编写代码极其容易出错，需要在任意步骤都要考虑是否出错。
 **/
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

int sockfd, connfd;

void stopServerRunning(int p)
{
    close(sockfd);
    printf("Close Server\n");
    exit(0);
}

int main() {
    char buff[BUFFSIZE];    // 用于收发数据

    /**
     * 创建套接字 
     */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Create socket error(%d):%s\n", errno, strerror(errno));
        return -1;
    }

    /**
     * 绑定ip和端口 
     */
    struct sockaddr_in servaddr; // 用于存放ip和端口的结构
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(6000);
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
        printf("Bind error(%d): %s\n", errno, strerror(errno));
        return -1;
    }

    /**
     * 监听 
     */
    if(listen(sockfd, 1024) == -1) {
        printf("Listen error(%d): %s\n", errno, strerror(errno));
        return -1;
    }
    printf("Listening...\n");

    /**
     * 循环等待客户端到来 
     */
    while(true) {
        signal(SIGINT, stopServerRunning);    // 这句用于在输入Ctrl+C的时候关闭服务器
        connfd = accept(sockfd, NULL, NULL);
        if(connfd == -1) {
            printf("Accept error(%d): %s\n", errno, strerror(errno));
            return -1;
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
    return 0;
}


