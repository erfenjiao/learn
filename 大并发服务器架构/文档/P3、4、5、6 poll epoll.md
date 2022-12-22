

IO复用：poll 、select、epoll

[TOC]

# poll

![3.1](/home/erfenjiao/learn/大并发服务器架构/截图/P3/3.1.png)

参数一：结构体指针，将结构体数组的首地址传递进来

参数二：监听的文件描述符个数

参数三：超时时间

## poll使用的基本流程

![3.2](/home/erfenjiao/learn/大并发服务器架构/截图/P3/3.2.png)

上图代码有为考虑到的问题，

1 假如应用层发送过来的数据包，设它需要两次read

read可能并没有把connfd所对应的接收缓冲区的数据都读完，那么connfd仍然处于活跃的状态，在下一次poll的时候继续读取。

解决：我们应该把读到的数据保存在connfd的应用层缓冲区

2 write 对请求进行应答，假如需要发送10000个字节，实际只发送了1000，可能的原因：发送缓冲区可能已满

解决：write 发送的时候，我们也应该有应用的发送缓冲区，且非阻塞

POLLOUT事件触发条件：connfd的发送缓冲区不满的时候

3 假如不是在if中触发，而是在开头“关注connfd的POLLIN事件”的同时，也关注POLLOUT事件， 刚开始的时候，是不需要发送事件的，但是发送缓冲区此时未满，会一直触发POLLOUT事件，造成 **busy-loop** 忙等待

poll 相当于 epoll 的 LT 状态，电平

## signal

```c
signal(SIGPIPE, SIG_IGN); //忽略SIGPIPE信号
```

![3.3](/home/erfenjiao/learn/大并发服务器架构/截图/P3/3.3.png)

SIGPIPE信号会使服务器中断

```c++
signal(SIGCHLD, SIG_IGN); //避免僵尸进程
```



## TIME_WAIT状态对大并发服务器的影响

应该避免，TIME_WAIT会使内核在一段时间内保留一些资源

如果服务器主动断开连接（先与client调用close），会进入TIME_WAIT状态

怎样避免：在协议上，让客户端主动连接，把TIME_WAIT状态分散到大量客户端

如果客户端不活跃了，一些恶意的客户端不断开连接，长期占用服务器资源，所以服务器需要针对这种情况，有机制来踢掉不活跃的close。

## EMFILE错误处理

accept（）打开的文件达到了上限，就会产生此类错误，解决办法：

![3.4](/home/erfenjiao/learn/大并发服务器架构/截图/P3/3.4.png)



1 2 3 治标不治本

设置阈值，比如最大20000,当到达19000时停止读取，但系统的上限很难确定

退出程序无法满足服务器 7*24 小时不间断运行

5  edge trigger 边沿触发，但这种方法也不可取

6  

````c++
int idlefd = open("/dev/null", O_RDONLY | O_CLOEXEC); //准备一个空闲的文件描述符

if (pollfds[0].revents & POLLIN)
{
    peerlen = sizeof(peeraddr);
    connfd = accept4(listenfd, (struct sockaddr*)&peeraddr,
                     &peerlen, SOCK_NONBLOCK | SOCK_CLOEXEC);

    if (connfd == -1)
        ERR_EXIT("accept4");

    /* 
			if (connfd == -1)
			{
				if (errno == EMFILE)
				{
					close(idlefd); // 关闭
					idlefd = accept(listenfd, NULL, NULL);
					close(idlefd);
					idlefd = open("/dev/null", O_RDONLY | O_CLOEXEC); //重新预备一个新的空白的文件描述符
					continue;
				}
				else
					ERR_EXIT("accept4");
			}
*/

    pfd.fd = connfd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    pollfds.push_back(pfd);
    --nready;

    std::cout<<"ip="<<inet_ntoa(peeraddr.sin_addr)<<
        " port="<<ntohs(peeraddr.sin_port)<<std::endl;
    if (nready == 0)
        continue;
}
````





# EPOLL

## 核心函数

![3.5](/home/erfenjiao/learn/大并发服务器架构/截图/P3/3.5.png)



## epoll LT

![3.6](/home/erfenjiao/learn/大并发服务器架构/截图/P3/3.6.png)

处理监听套接字事件，不能在这个时候关注EPOLLOUT事件，否则会陷入busy-loop

处理已连接套接字事件：

​		不需要和poll一样遍历，因为epoll_wait返回的，都是活跃的

​		read：缓冲区分为应用层缓冲区和内核层缓冲区，即使应用层缓冲区没有接收完，也仍然会触发EPOLLIN事件

​		write：发送缓冲区，发送10000个字节，有可能没有完全发送完，则进入if判断，处于高电平状态

### man epoll_create

```c++
NAME
       epoll_create, epoll_create1 - open an epoll file descriptor

SYNOPSIS
       #include <sys/epoll.h>

       int epoll_create(int size);
       int epoll_create1(int flags);
DESCRIPTION
    epoll_create1()
       If flags is 0, then, other than the fact that the obsolete  size  argu‐
       ment  is  dropped,  epoll_create1() is the same as epoll_create().  The
       following value can be included in flags to obtain different behavior:

       EPOLL_CLOEXEC
              Set the close-on-exec (FD_CLOEXEC) flag on the new file descrip‐
              tor.   See  the description of the O_CLOEXEC flag in open(2) for
              reasons why this may be useful.
```

详情实例参见代码/P5

poll模型

每次调用poll函数的时候，都需要把监听套接字与已连接字所感兴趣的事件数组拷贝到内核，效率比较低

epoll

不需要拷贝，在epoll_create创建后，已经在内核维护了一组数据

## epoll ET

什么时候关注EPOLLOUT事件？

![3.7](/home/erfenjiao/learn/大并发服务器架构/截图/P3/3.7.png)

处理监听套接字事件：处于高电平

处理已连接套接字事件：

​		EPOLLIN事件到来，接收缓冲区有数据，必须全部接收走，如果没有接收完，下一次epoll_wait事件不会触发，因为一直处于高电平

​		if：发送缓冲区满了，处于低电平状态，等待数据被接收走，又处于高电平状态

​		write：将应用层的数据拷贝到内核层

EPOLLIN事件

内核中的接收缓冲区 为空 低电平

​                                     不为空 高电平

EPOLLOUT事件

 内核中的发送缓冲区 不满 高电平

​                                       满  低电平

​        

# select poll epoll对比

## 原理

![3.8](/home/erfenjiao/learn/大并发服务器架构/截图/P3/3.8.png)



### select 

文件描述符，最多1024

​				操作：fd_set fd_insert

### poll

代码P5/echosrv_poll.cpp

```c++
while (1)
	{
		nready = poll(&*pollfds.begin(), pollfds.size(), -1); //将数据从用户空间拷贝到内核空间
			。。。。。。
    		

		//std::cout<<pollfds.size()<<std::endl;
		//std::cout<<nready<<std::endl;
		for (PollFdList::iterator it=pollfds.begin()+1;  //遍历
			it != pollfds.end() && nready >0; ++it)
		{
			。。。。。。
```

### epoll

使用共享内存减少开销

```c++
int epollfd;
epollfd = epoll_create1(EPOLL_CLOEXEC); //

struct epoll_event event;
event.data.fd = listenfd;
event.events = EPOLLIN   /* | EPOLLET*/;
epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event); //添加到共享内存中
```

## 一个进程所能打开的最大连接数

![3.9](/home/erfenjiao/learn/大并发服务器架构/截图/P3/3.9.png)

##  FD剧增后带来的IO效率问题

![3.10](/home/erfenjiao/learn/大并发服务器架构/截图/P3/3.10.png)



## 消息传递方式

![3.11](/home/erfenjiao/learn/大并发服务器架构/截图/P3/3.11.png)

是否认为epoll比poll、select效率更高？error

