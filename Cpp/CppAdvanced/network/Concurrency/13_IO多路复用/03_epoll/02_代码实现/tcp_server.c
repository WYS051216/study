#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

int main(int argc, const char *argv[])
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
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 设置端口复用
    int optval = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // 绑定端口
    int ret = bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret == -1)
    {
        perror("bind error");
        exit(1);
    }

    // 设置监听
    ret = listen(lfd, 64);
    if (ret == -1)
    {
        perror("listen error");
        exit(1);
    }

    // 创建一个epoll实例
    int epfd = epoll_create(1);
    if (epfd == -1)
    {
        perror("create error");
        exit(0);
    }
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = lfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

    struct epoll_event evs[1024];
    int size = sizeof(evs) / sizeof(evs[0]);
    while (1)
    {
        int num = epoll_wait(epfd, evs, size, -1);
        printf("num = %d\n", num);
        for (int i = 0; i < num; i++)
        {
            int fd = evs[i].data.fd;
            if (fd == lfd)
            {
                int cfd = accept(fd, NULL, NULL);
                ev.events = EPOLLIN;
                ev.data.fd = cfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
            }
            else{
                char buf[1024];
                int len = recv(fd, buf, sizeof(buf), 0);
                if(len == -1)
                {
                    perror("recv error");
                    exit(1);
                }else if(len == 0)
                {
                    printf("客户端已经断开连接...\n");
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    break;
                }
                printf("read buf = %s\n", buf);
                for(int i = 0; i < len; i++)
                {
                    buf[i] = toupper(buf[i]);
                }
                printf("after buf = %s\n", buf);
                ret = send(fd, buf, strlen(buf) + 1, 0);
                if(ret == -1)
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