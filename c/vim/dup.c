/*************************************************************************
	> File Name: dup.c
	> Author: erfenjiao 
	> Mail: 630166475@qq.com 
	> Created Time: 2021年03月31日 星期三 22时17分15秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

#define FNAME "/tmp/out" 
int main()
{
    // puts("hello!");  终端上输出hello   
    //改进：让hello不在终端上输出，而是输出到一个文件中 ,输出重定向
    //文件描述符 优先使用当前可用范围内最小的，标准输出的号码为1
    int fd;  //是数组下标，整型数

    fd = open(FNAME,O_WRONLY | O_CREAT | O_TRUNC,0600);
    if(fd < 0)
    {
        perror("open()");
        exit(1);
    }
    //close(1);   //不原子的话，可能close刚关闭，没等把这个关闭的位置给本文件，就有另一个文件占了此位置，会使重定向到别的文件中
    //dup(fd);
    //
    //
    //改进操作二 ，是原子操作，但依旧没有回避fd等于1的情况
    duo2(fd,1);
    if(fd != 1)
    {
        close(fd);
    }
    //可以输出到文件，但是有bug
    //假如系统中那个数组默认关闭1，那么程序从上往下，fd 首先是 1 ，然后close 1 ， fd 已经释放了，那么 dup 这个操作无效，最后又关闭了一次
    //关键原因：这两步操作不原子




    puts("hello");





    exit(0);
}
