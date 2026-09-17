#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<pthread.h>

//信息结构体
struct SockInfo
{
    struct sockaddr_in addr;
    int fd;
};
struct SockInfo infos[512];

void* working(void* arg);

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
    //初始化结构体数组
    int max = sizeof(infos)/sizeof(infos[0]);
    for(int i = 0; i < max; i++)
    {
        bzero(&infos[i], sizeof(infos[i]));
        infos[i].fd = -1;
    }

    //4.阻塞并等待客户端连接
    struct sockaddr_in caddr;   //用来存储连进来的客户端的IP和端口
    int addrlen = sizeof(struct sockaddr_in);
    //一旦有人连进来，它会返回一个全新的文件描述符cfd
    while(1)
    {
        struct SockInfo* pinfo;
        for(int i = 0; i < max; i++)
        {
            if(infos[i].fd == -1)
            {
                pinfo = &infos[i];
                break;
            }
        }
        int cfd = accept(fd, (struct sockaddr*)&pinfo->addr, &addrlen);
        pinfo->fd = cfd;
        if(cfd == -1)
        {
            perror("accept");
            break;
        }
        //创建一个对应的子线程
        pthread_t tid;
        pthread_create(&tid, NULL, working, pinfo);
        pthread_detach(tid);
    }
    close(fd);
    return 0;
}

void* working(void* arg)
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
    pinfo->fd = -1;
    return NULL;
}
