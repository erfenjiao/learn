/*************************************************************************
	> File Name: ab.c
	> Author: erfenjiao 
	> Mail: 630166475@qq.com 
	> Created Time: 2021年03月31日 星期三 21时36分07秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main()
{
    putchar('a');
    write(1,"b",1);
    
    putchar('a');
    write(1,"b",1);


    putchar('a');
    write(1,"b",1);

    //输出结果 ：bbbaaa
    exit(0);

}
