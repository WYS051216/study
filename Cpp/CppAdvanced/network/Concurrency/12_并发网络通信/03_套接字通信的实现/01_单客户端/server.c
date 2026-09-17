#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>

int main()
{
    //1.创建监听的套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    //判断套接字是否创建成功
    if(fd == -1)
    {
        perror("socket");
        return -1;
    }
    //2.绑定本地的IP port
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = INADDR_ANY;   //0 = 0.0.0.0(自动去读本地的网卡对应的IP地址)
    //bind将套接字fd和上面指定的IP和端口绑定在一起
    int ret = bind(fd, (struct sockaddr*)&saddr, sizeof(saddr));
    if(ret == -1)
    {
        perror("bind");
        return -1;
    }
    //设置监听
    //128是“待处理连接队列”的最大长度
    ret = listen(fd, 128);
    //判断是否监听成功
    if(ret == -1)
    {
        perror("listen");
        return -1;
    }
    //4.阻塞并等待客户端连接
    struct sockaddr_in caddr;   //用来存储连进来的客户端的IP和端口
    int addrlen = sizeof(caddr);
    //一旦有人连进来，它会返回一个全新的文件描述符cfd
    int cfd = accept(fd, (struct sockaddr*)&caddr, &addrlen);
    if(cfd == -1)
    {
        perror("accept");
        return -1;
    }
    //连接建立成功，答应客户端的IP和端口信息
    char ip[32];
    printf("客户端的IP: %s, 端口: %d\n",
            inet_ntop(AF_INET, &caddr.sin_addr.s_addr, ip, 
                sizeof(ip)), ntohs(caddr.sin_port));
    //5.通信
    while(1)
    {
        //接收数据
        char buff[1024];
        int len = recv(cfd, buff, sizeof(buff), 0);
        if(len > 0)
        {
            printf("client say: %s\n", buff);
            send(cfd, buff, len, 0);
        }
        else if(len == 0)
        {
            printf("客户端已经断开了连接...\n");
            break;
        }
        else{
            perror("recv");
            break;
        }
    }
    //关闭文件描述符
    close(fd);
    close(cfd);
    return 0;
}
