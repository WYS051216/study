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
    //创建子进程
    pid_t pid = fork();
    if(pid > 0)
    {
        //父进程，完成向管道1中写数据
        //打开管道文件
        int rfd = -1;
        if((rfd = open("myfifo1", O_RDONLY)) == -1)
        {
            perror("open error");
            return -1;
        }
        //准备要写入的数据
        char rbuf[128] = "";
        while(1)
        {
            //将容器清空
            bzero(rbuf, sizeof(rbuf));

            //从管道文件中读取数据
            read(rfd, rbuf, sizeof(rbuf));
            printf("收到数据为:%s\n", rbuf);
            if(strcmp(rbuf, "quit") == 0)
            {
                break;
            }
        }
        close(rfd);
        wait(NULL);
    }
    else if(pid == 0){
        //子进程，完成从管道2中读取数据
        //打开管道文件
        int sfd = -1;
        if((sfd = open("myfifo2", O_WRONLY)) == -1)
        {
            perror("open error");
            return -1;
        }
        //准备要写入的数据
        char wbuf[128] = "";
        while(1)
        {
            //从终端输入数据
            fgets(wbuf, sizeof(wbuf), stdin);
            //将换行换成'\0'
            wbuf[strlen(wbuf) - 1] = '\0';
            //将数据写入管道
            write(sfd, wbuf, strlen(wbuf));
            if(strcmp(wbuf, "quit") == 0)
            {
                break;
            }
        }
        close(sfd);
        //回收子进程资源
        exit(EXIT_SUCCESS);
    }
    else{
        perror("fork error");
        return -1;
    }
    return 0;
}