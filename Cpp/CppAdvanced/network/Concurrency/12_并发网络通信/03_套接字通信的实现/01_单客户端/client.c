#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>

int main()
{
    //1.创建通信的套接字
    //AF_INET: 使用IPv4协议
    //SOCK_STREAM:使用流式协议（即TCP）
    //0：使用协议族中默认的协议（TCP）
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    //判断套接字是否创建成功，失败通常返回-1
    if(fd == -1)
    {
        //打印错误信息
        perror("socket");
        return -1;
    }
    //2.连接服务器IP port
    struct sockaddr_in saddr;   //定义一个结构体变量，用于保存服务器地址信息
    saddr.sin_family = AF_INET; //指定协议族为IPv4
    saddr.sin_port = htons(9999);   //指定端口为9999，并将主机字节序转化为网络大端你字节序
    inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr.s_addr);    //将主机字节序转为网络大端字节序，并且存储在s_addr里面
    //发起连接请求
    //参数：文件描述符，服务器地址结构体指针，结构体大小
    int ret = connect(fd, (struct sockaddr*)&saddr, sizeof(saddr));
    if(ret == -1)
    {
        perror("connect");      //如果连接失败，会报错
        return -1;
    }
    int number = 0;
    //3.循环通信
    while(1)
    {
        //发送数据
        char buff[1024];    //将格式化后的字符串存入buff,每次循环number自增
        sprintf(buff, "你好, hello, world, %d...\n", number++);
        //发送数据给服务器
        //strlen(buff) + 1 表示连同字符串结束符'\0'一起发出去
        send(fd, buff, strlen(buff) + 1, 0);
        //接收数据
        memset(buff, 0, sizeof(buff));
        //接收服务器传回的数据
        //recv是阻塞函数，如果没有数据传回来，程序会停在这里等待
        int len = recv(fd, buff, sizeof(buff), 0);
        if(len > 0)
        {
            printf("client say: %s\n", buff);
        }
        else if(len == 0)
        {
            printf("服务器端已经断开了连接...\n");
            break;
        }
        else{
            perror("recv");
            break;
        }
        sleep(1);
    }
    //关闭文件描述符
    close(fd);
    return 0;
}
