#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main()
{
    // 1. 创建通信的套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("socket error");
        return -1;
    }

    // 2. 连接服务器
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr)); 
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);      
    inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr.s_addr);

    int ret = connect(fd, (struct sockaddr*)&saddr, sizeof(saddr));
    if(ret == -1) {
        perror("connect error");
        return -1;
    }

    printf("--- 成功连接服务器，开始自动发送测试数据 --- \n");

    // 3. 循环通信：自动发送 hello, world, X
    char buff[1024];
    for(int i = 1; i <= 1024; i++) // 模拟发送1到1024
    {
        memset(buff, 0, sizeof(buff));
        
        // 自动拼装字符串，例如：hello, world, 1
        sprintf(buff, "hello, world, %d", i);

        // 发送数据给服务器
        send(fd, buff, strlen(buff) + 1, 0);

        // 接收服务器传回的大写数据
        memset(buff, 0, sizeof(buff));
        int len = recv(fd, buff, sizeof(buff), 0);
        
        if(len > 0) {
            printf("server say: %s\n", buff);
        } else {
            break;
        }
        
        sleep(1); // 稍微停顿0.1秒，避免发送过快粘包
    }

    // 4. 关闭文件描述符
    close(fd);
    return 0;
}