#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>

int main(int arg, const char *argv[])
{
    // 创建监听的套接字
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("socket error");
        exit(1);
    }
    // 绑定
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9999);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 本地多有的IP

    // 将监听套接字和服务器地址serv_addr绑定起来
    int ret = bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret == -1)
    {
        perror("bind error");
        exit(1);
    }
    // 监听
    //64：同时允许内核维护的已完成连接队列长度
    ret = listen(lfd, 64);
    if (ret == -1)
    {
        perror("listen error");
        exit(1);
    }
    //定义一个文件描述符集合
    fd_set redset;
    //将集合清空
    FD_ZERO(&redset);
    //将监听套接字lfd加入集合中
    FD_SET(lfd, &redset);
    //记录当前最大的文件描述符值
    int maxfd = lfd;
    while (1)
    {
        //每次调用select前复制集合
        fd_set tmp = redset;
        //调用select函数
        //函数作用：监听套接字可读-->有新客户端连接
                 //某个客户端套接字可读-->有客户端发来了数据 
        int ret = select(maxfd + 1, &tmp, NULL, NULL, NULL);
        //判断监听套接字是否就绪
        if (ret == -1) 
        {
            perror("select error");
            exit(1);
        }
        // 判断是不是用于监听的文件描述符
        if (FD_ISSET(lfd, &tmp))
        {
            // 接受客户端的连接，返回一个新的通信文件描述符cfd
            int cfd = accept(lfd, NULL, NULL);
            //把新客户端你加入监控集合
            FD_SET(cfd, &redset);
            //更新当前最大的文件描述符
            maxfd = cfd > maxfd ? cfd : maxfd;
        }
        //遍历所有的文件描述符
        for (int i = 0; i <= maxfd; i++)
        {
            //i != lfd：排除监听套接字，FD_ISSET：检查这个fd是否可读
            if (i != lfd && FD_ISSET(i, &tmp))
            {
                // 接受数据
                char buf[1024];
                int len = recv(i, buf, sizeof(buf), 0);
                if (len == -1)
                {
                    printf("recv error...\n");
                    exit(1);
                }
                else if (len == 0)
                {
                    printf("客户端已经断开连接...\n");
                    FD_CLR(i, &redset);
                    close(i);
                    break;
                }
                printf("read buf = %s\n", buf);
                // 小写转大写
                for (int j = 0; j < len; j++)
                {
                    buf[j] = toupper(buf[j]);
                }
                printf("after buf = %s\n", buf);
                //将转化后的数据发回客户端
                //i:客户端的套接字，buf:发送内容，len:发送字节数
                ret = send(i, buf, len, 0);
                //发送失败则报错退出
                if (ret == -1)
                {
                    perror("send error");
                    exit(1);
                }
            }
        }
    }
    close(lfd);
    return 0;
}