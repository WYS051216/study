#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<pthread.h>
#include "threadpool.h"

//信息结构体
struct SockInfo
{
    struct sockaddr_in addr;
    int fd;
};

typedef struct PoolInfo{
    ThreadPool* p;
    int fd;
}PoolInfo;

struct SockInfo infos[512];

void working(void* arg);

void acceptConn(void* arg);

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
    //创建线程池
    ThreadPool *pool = threadPoolCreate(3, 8, 100);
    PoolInfo* info = (PoolInfo*)malloc(sizeof(PoolInfo));
    info->p = pool;
    info->fd = fd;
    threadPoolAdd(pool, acceptConn, info);

    pthread_exit(NULL);
    return 0;
}

void acceptConn(void* arg)
{
    PoolInfo* poolInfo = (PoolInfo* )arg;
    int addrlen = sizeof(struct sockaddr_in);
    //一旦有人连进来，它会返回一个全新的文件描述符cfd
    while(1)
    {
        struct SockInfo* pinfo;
        pinfo = (struct SockInfo*)malloc(sizeof(struct SockInfo));
        pinfo->fd = accept(poolInfo->fd, (struct sockaddr*)&pinfo->addr, &addrlen);
        if(pinfo->fd == -1)
        {
            perror("accept");
            break;
        }
        //添加通信的任务
        threadPoolAdd(poolInfo->p, working, pinfo);
    }
    close(poolInfo->fd);
}

void working(void* arg)
{
    struct SockInfo* pinfo = (struct SockInfo*)arg;
    //连接建立成功，答应客户端的IP和端口信息
    char ip[32];
    printf("客户端的IP: %s, 端口: %d\n",
            inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, ip, 
                sizeof(ip)), ntohs(pinfo->addr.sin_port));
    //5.通信
    while(1)
    {
        //接收数据
        char buff[1024];
        int len = recv(pinfo->fd, buff, sizeof(buff), 0);
        if(len > 0)
        {
            printf("client say: %s\n", buff);
            send(pinfo->fd, buff, len, 0);
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
    close(pinfo->fd);
}