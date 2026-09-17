#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main()
{
    // 1. 创建用于通信的套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1)
    {
        perror("socket");
        exit(0);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
    int ret = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1)
    {
        perror("connect");
        exit(0);
    }
    while(1)
    {
        //定义一个字符数组，用来存放从键盘输入的内容，从socket读回来的内容
        char recvBuf[1024];
        //从键盘读
        fgets(recvBuf, sizeof(recvBuf), stdin);
        //将recvBuf中的数据写到文件描述符fd对应的对象里
        write(fd, recvBuf, strlen(recvBuf)+1);
        //从fd中读取数据到recvBuf
        read(fd, recvBuf, sizeof(recvBuf));
        printf("recv buf: %s\n", recvBuf);
        sleep(1);
    }
    close(fd); 
    return 0;
}