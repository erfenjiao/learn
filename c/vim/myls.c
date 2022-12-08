/*************************************************************************
	> File Name: myls.c
	> Author: erfenjiao 
	> Mail: 630166475@qq.com 
	> Created Time: 2021年04月01日 星期四 16时46分27秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>

static off_t flen(const char * fname)  //static  禁止外部扩展  该函数返回文件大小
{                                     // const  只读不写
    struct stat statres;
    if(stat(fname,&statres) < 0)
    {
        perror("stat()");
        exit(1);
    }
    return statres.st_size; 
}



int main(int argc,char ** argv)
{
    if(argc < 2)
    {
        fprintf(stderr,"Usage...\n");
        exit(1);
    }

    printf("%lld",(long long)flen(argv[1]));  // off_t 类型

    exit(0);
}
