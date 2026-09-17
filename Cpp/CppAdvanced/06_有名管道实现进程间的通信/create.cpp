#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>

int main(int argc, char *argv[])
{
    //该文件主要负责创建管道文件，注意：如果管道文件已经存在，那么mkfifo函数会报错
    if(mkfifo("myfifo1", 0664) == -1)
    {
        perror("mkfifo error");
        return -1;
    }
    if(mkfifo("myfifo2", 0664) == -1)
    {
        perror("mkfifo error");
        return -1;
    }
    printf("管道文件创建成功\n");
    return 0;
}