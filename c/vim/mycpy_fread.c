/*************************************************************************
	> File Name: mycpy.c
	> Author: 
	> Mail: 
	> Created Time: 2021年03月29日 星期一 16时21分28秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>


#define BUFSIZE 1024
int main(int argc,char ** argv)
{
    FILE *fps,*fpd;
    int ch;                 //有无符号是未定义行为 ， 任何标准C都没有定义
    char  buf[BUFSIZE];
    int n;
    if(argc < 3)            //如果命令行的传参不是三个的话 即有此文件名 、准备复制到其他文件的文件 、 准备复制前一个文件的文件
    {
        fprintf(stderr,"Usage: %s <src_file> <dest_file>\n",argv[0]);
        exit(1);
    }

    fps = fopen(argv[1],"r");   //文件一定存在，不存在就要报错，所以用 r; 如果第一个文件打开成功，第二个文件打开失败，会产生内存泄露
    if(fps == NULL)             //因为第一个文件的指针fps没有释放，程序在进行到第二个文件报错后会停止，导致第一个指针永远释放不了
    {
        perror("fopen()");
        exit(1);
    }
    fpd = fopen(argv[2],"w");
    if(fpd == NULL)
    {
        fclose(fps);       //要关闭，原因如上（22 、23 line）
        perror("fopen()");
        exit(1);
    }

//注意，从mycpy_fgets.c 到 mycpy_fread.c  仅有这个循环发生改变,我使上个文件的循环保留，看一下区别
//    while(fgets(buf,BUFSIZE,fps) != NULL)  //返回的是一个空指针，而不是  EOF  ,用于跳出循环
//    {                                // fgets() returns s on success, and NULL on error or when end of file occurs while no characters have been read.
        //ch = fgetc(fps);
        //if(ch == EOF)
        //    break;
        //fputc(ch,fpd);   //可以检验，但一般不会出错
        

        //fgets(buf,BUFSIZE,fps);                      //char *fgets(char *s, int size, FILE *stream);
//        fputs(buf,fpd);

//    }
      while(n = fread(buf,1,BUFSIZE,fps) > 0)
     {
         //  fwrite(buf,1,BUFSIZE,fpd);   //这样的书写会出错,原因，读文件不能保证都是BUFSIZE的整倍数，所以必须判断返回值
         fwrite(buf,1,n,fpd);
     }
    fclose(fpd);
    fclose(fps);

    exit(0);
}                               
