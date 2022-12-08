/*************************************************************************
	> File Name: mycpy2.c
	> Author: erfenjiao 
	> Mail: 630166475@qq.com 
	> Created Time: 2021年03月31日 星期三 17时22分16秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#define BUFSIZE 1024

int main(int argc,char ** argv)
{
    int sfd,dfd;
    char buf[BUFSIZE];
    int len = 0,ret = 0;
    if(argc <3)
    {
        fprintf(stderr,"Usage...\n");
        exit(1);
    }
    sfd = open(argv[1],O_RDONLY);
    if(sfd < 0)
    {
        perror("open()");
        exit(1);
    }
    dfd = open(argv[2],O_WRONLY | O_CREAT | O_TRUNC , 0600);
    if(dfd < 0)
    {
        close(sfd);
        perror("open()");
        exit(1);
    }

    pos = 0;

    while(len > 0)
    {
        len = read(sfd,buf+pos,BUFSIZE);
        if(len < 0)
        {
            perror("read()");
            break;;
        }
        if(len ==0)
            break;
        ret = write(dfd,buf,len);  //假如 len 读取了十个字节，而 write 只写入了三个，然后进入了下个循环，read 会覆盖原空间，也就是说，凭空少了七个字节


        if(ret < 0)
        {
            perror("write()");
            exit(1);
        
        }
        len-=ret;
        pos+=ret;
    }
    



    close(dfd);
    close(sfd);

    exit(0);
}
