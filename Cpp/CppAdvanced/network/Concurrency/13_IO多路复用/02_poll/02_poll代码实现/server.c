#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <poll.h>

int main()
{
    // 1.创建套接字
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("socket");
        exit(0);
    }
    // 2. 绑定 ip, port
    struct sockaddr_in addr;
    addr.sin_port = htons(9999);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    int ret = bind(lfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("bind");
        exit(0);
    }
    // 3. 监听
    ret = listen(lfd, 100);
    if (ret == -1)
    {
        perror("listen");
        exit(0);
    }

    //定义pollfd数组，最多管理1024个文件描述符
    struct pollfd fds[1024];
    //初始化数组
    for (int i = 0; i < 1024; ++i)
    {
        fds[i].fd = -1;     //这个位置当前还没有舒勇
        fds[i].events = POLLIN; //对每个位置都关注“可读事件”
        //POLLIN的意思是:①对监听套接字来说:有新连接到来;②对已连接套接字来说:有数据可读
    }
    //把监听描述符放到第0个位置，后面poll就会一直监视它
    fds[0].fd = lfd;

    //定义当前最大下标
    int maxfd = 0;
    while (1)
    {
        //调用poll阻塞等待事件，-1表示永久阻塞，直到有事件发生
        ret = poll(fds, maxfd + 1, -1);
        //检查poll是否出错
        if (ret == -1)
        {
            perror("select");
            exit(0);
        }
        //判断fds[0]这个文件描述符上，是否发生了可读事件
        //revents是poll返回后填写的“实际发生的事件”
        //看看 fds[0].revents 里面是否包含 POLLIN 这个标志
        if (fds[0].revents & POLLIN)
        {
            //定义客户端地址结构，用于保存客户端的IP和端口信息
            struct sockaddr_in sockcli;
            //定义地址长度
            int len = sizeof(sockcli);
            //接受新连接，从监听套接字lfd接收一个新的客户端连接
            //connfd是：新的已连接套接字，后续和这个客户端通信都通过它进行
            //accept成功后：sockcli里会保存客户端地址信息，connfd会被加入fds数组
            int connfd = accept(lfd, (struct sockaddr *)&sockcli, &len);
            //寻找空闲槽位存放新连接
            int i;
            for (i = 0; i < 1024; ++i)
            {
                //在fds数组中寻找一个空闲位置，然后把新连接connfd放进去
                if (fds[i].fd == -1)
                {
                    fds[i].fd = connfd;
                    break;
                }
            }
            //更新最大下标
            maxfd = i > maxfd ? i : maxfd;
        }
        //遍历所有客户端连接
        for (int i = 1; i <= maxfd; ++i)
        {
            //判断某个客户端是否可读，如果某个客户端套接字发生了“可读事件”，说明：客户端发来了数据，或者客户端关闭了连接
            if (fds[i].revents & POLLIN)
            {
                //定义读缓冲区，准备一个128字节的缓冲区来接收客户端数据
                char buf[128];
                //从客户端读取数据
                int ret = read(fds[i].fd, buf, sizeof(buf));
                if (ret == -1)
                {
                    perror("read");
                    exit(0);
                }
                else if (ret == 0)
                {
                    printf("对方已经关闭了连接...\n");
                    close(fds[i].fd);
                    fds[i].fd = -1;
                }
                else
                {
                    printf("客户端say: %s\n", buf);
                    write(fds[i].fd, buf, strlen(buf) + 1);
                }
            }
        }
    }
    close(lfd);
    return 0;
}