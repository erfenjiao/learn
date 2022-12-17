#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <poll.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <vector>
#include <iostream>

#define ERR_EXIT(m) \
	do \
	{ \
		perror(m); \
		exit(EXIT_FAILURE); \
	}while(0)

typedef std::vector<struct pollfd> PollFdList

int main() {
	signal(SIGPIPE, SIG_IGN); // ?
	signal(SIGCHLD, SIG_IGN); // ?
	
	int listenfd;
	listenfd = socket(PF_INET, SOCK_STREAM | SOCK_NOBLOCK | SOCK_CLOEXEC, IPROTO_TCP);
	/*
	 * 编程模型: nonblocking socket + I/O 复用
	 * SOCK_NOBLOCK : 非阻塞模式
	 * SOCK_CLOEXEC : 进程被替换的时候，关闭，对后续进程造成最小影响
	 * */
	if (listenfd < 0) {
		ERR_EXIT("socket");
	}
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));





}











