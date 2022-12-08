/*************************************************************************
	> File Name: ftype.c
	> Author: erfenjiao 
	> Mail: 630166475@qq.com 
	> Created Time: 2021年04月01日 星期四 20时05分28秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>

static int ftype(const char *fname)  //
//虽然文件类型是字母，但不建议返回值设成char，因为他不安全，不能确定正负号
{
  struct stat statres;
    if(stat(fname,&statres)<0)
    {
        perror("stst()");
        exit(1);
    }
    if(S_ISREG(statres.st_mode))
        return '-';
    else if(S_ISDIR(statres.st_mode))
        return 'd';
    else if(S_ISSOCK(statres.st_mode))
        return 's';
    else if(S_ISBLK(statres.st_mode))
        return 'b';
    else 
        return '?';
}

int main(int argc,char** argv)
{
    if(argc < 2)
    {
        fprintf(stderr,"Usage...\n");
        exit(1);
    }

    printf("%c",ftype(argv[1]));


    exit(0);
}
