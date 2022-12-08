/*************************************************************************
	> File Name: few.c
	> Author: amoscykl
	> Mail: amoscykl@163.com 
	> Created Time: 2021年03月21日 星期日 16时55分26秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main()
{
	pid_t pid;
	puts("Begin!");

	fflush(NULL);
	pid=fork();
	if(pid<0)
	{
		perror("fork()");
		exit(1);
	}
	if(pid==0)
	{
		execl("/bin/date","data","+%s",NULL);
		perror("execl()");
		exit(1);
	}
	wait(NULL);
	puts("End!");

	exit(0);


}
