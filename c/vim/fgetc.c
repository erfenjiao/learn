/*************************************************************************
	> File Name: fgetc.c
	> Author: erfenjiao 
	> Mail: 630166475@qq.com 
	> Created Time: 2021年03月29日 星期一 17时09分08秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>


int main(int argc , char ** argv)
{
    FILE *fp;
    int count = 0;
    if(argc < 2 )                         //命令行传参有误
    {
        fprintf(stderr,"Usage...\n");     //...请参考 mycpy.c 文件中判断参数个数时的信息
        exit(1);
    }
    
    fp = fopen(argv[1],"r");              //以只读打开文件
    if(fp ==NULL)
    {
        perror("fopen()");
        exit(1);
    }
    while(fgetc(fp) != EOF)
   {
        count++;                          //计数器
   }

    printf("count = %d\n",count);

    fclose(fp);







    exit(0);
}
