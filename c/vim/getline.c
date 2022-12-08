/*************************************************************************
	> File Name: getline.c
	> Author: erfenjiao 
	> Mail: 630166475@qq.com 
	> Created Time: 2021年03月30日 星期二 22时07分58秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc,char ** argv)
{
    FILE * fp;
    char *linebuf;
    size_t linesize;
    if(argc < 2)
    {
        fprintf(stderr,"Usage...\n");
        exit(1);
    }
    fp = fopen(argv[1],"r");
    if(fp == NULL)
    {
        perror("foprn()");
        exit(1);
    }
    
    //初始化，非常必须!!!如果不加，会出现错误结果  (45 \n 4195872) 这个错误原因是对变量没有做初始化 
    linebuf = NULL;       //用这两句来判断到底用过没用过
    linesize = 0;

    while(1)
    {
        if(getline(&linebuf,&linesize,fp) < 0)  //也防止出现异常，正常情况是大于等于零的，但不排除有负数出现
        {                                       
            break;
        }
        printf("%d\n",strlen(linebuf));
        printf("%d\n",linesize);
    }





    //free(linebuf);  不太建议，例如在c++中，用new申请，就不能用free释放
    fclose(fp);
    exit(0);
}

