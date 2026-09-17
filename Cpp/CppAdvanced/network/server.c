#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // 提供 close, read, write 等系统调用
#include <string.h>
#include <arpa/inet.h>   // 提供 socket, bind, listen, inet_ntop 等网络函数
#include <sys/epoll.h>   // 【核心】提供 epoll_create, epoll_ctl, epoll_wait

#define MAX_CLIENT 512   // 定义最大允许同时连接的客户端数量
#define MAX_EVENTS 1024  // epoll_wait 一次最多能处理多少个就绪事件

// 定义一个结构体，用来存客户端的地址信息和文件描述符(fd)
struct SockInfo
{
    struct sockaddr_in addr;
    int fd;
};

// 全局数组：相当于一个“在线用户列表”
// 索引是下标，内容是客户端信息
struct SockInfo infos[MAX_CLIENT];

//添加客户端函数
void add_client(int fd, struct sockaddr_in addr) {
    // 遍历数组，找一个空位（fd为-1表示该位置是空的）
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (infos[i].fd == -1) {
            infos[i].fd = fd;      // 填入文件描述符
            infos[i].addr = addr;  // 填入地址信息
            return; // 找到位置存好后，直接返回
        }
    }
}

//移除客户端函数
void remove_client(int fd) {
    // 遍历数组，找到那个要断开的 fd
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (infos[i].fd == fd) {
            infos[i].fd = -1; // 把它标记为 -1，表示这个位置空出来了，别人可以用
            return;
        }
    }
}

//群发广播
void broadcast_msg(int sender_fd, char *buff, int len) {
    // 遍历整个列表
    for (int i = 0; i < MAX_CLIENT; ++i) {
        // 1. 位置必须有效 (fd != -1)
        // 2. 接收者不能是发送者自己 (fd != sender_fd)
        if (infos[i].fd != -1 && infos[i].fd != sender_fd) {
            // 发送数据
            send(infos[i].fd, buff, len, 0);
        }
    }
}

//服务器初始化
int main()
{
    // 1. 创建监听套接字 (买个手机)
    // AF_INET: IPv4, SOCK_STREAM: TCP协议
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) { perror("socket"); return -1; }

    // 2. 设置端口复用 (非常重要！)
    // 防止服务器重启时报 "Address already in use" 错误
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 3. 绑定 IP 和 端口 (插上电话卡)
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);       // 端口号 9999
    saddr.sin_addr.s_addr = INADDR_ANY; // 绑定本机所有网卡 IP
    if (bind(lfd, (struct sockaddr*)&saddr, sizeof(saddr)) == -1) {
        perror("bind"); return -1;
    }

    // 4. 设置监听 (待机，准备接电话)
    // 128 是等待队列的长度
    if (listen(lfd, 128) == -1) { 
        perror("listen"); return -1; 
    }

    // 5. 初始化全局数组
    // 把所有位置都置为 -1，表示现在没人在线
    for (int i = 0; i < MAX_CLIENT; i++) {
        infos[i].fd = -1;
    }

    //Epoll 示例创建(核心)
    // 1. 创建 epoll 实例 (雇佣一个秘书)
    // 参数 1 只是个建议值，现在Linux内核会自动调整，只要大于0就行
    int epfd = epoll_create(1);
    if (epfd == -1) { 
        perror("epoll_create"); 
        return -1; 
    }

    // 2. 将“监听套接字 lfd”上树 (告诉秘书：帮我盯着主电话机)
    struct epoll_event ev;
    ev.events = EPOLLIN; // EPOLLIN 表示关注“有数据读入”事件（即有新连接）
    ev.data.fd = lfd;    // 记录我们要盯着谁
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev); // ADD 表示添加监控

    // 3. 准备一个数组，用来接收 epoll_wait 返回的就绪事件
    struct epoll_event events[MAX_EVENTS];
    printf("Epoll 服务器启动，等待连接...\n");
    
    //事件循环
    while (1)
    {
        // 1. 阻塞等待 (老板睡觉，等秘书叫醒)
        // -1 表示永久阻塞，直到有事件发生
        // ret 返回的是：实际发生了几个事件
        int ret = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (ret == -1) { 
            perror("epoll_wait"); 
            break; 
        }

        // 2. 遍历处理所有发生的事件
        for (int i = 0; i < ret; i++)
        {
            // 取出当前发生事件的 socket fd
            int curfd = events[i].data.fd; 

            // === 情况 A: 如果是 lfd 响了 ===
            // 说明：有新客户端要连接
            if (curfd == lfd)
            {
                struct sockaddr_in client_addr;
                socklen_t addrlen = sizeof(client_addr);
                // 接受连接，生成一个新的通信用 fd (cfd)
                int cfd = accept(lfd, (struct sockaddr*)&client_addr, &addrlen);

                if (cfd == -1) { perror("accept"); continue; }

                // 打印一下是谁连进来了
                char ip[32];
                printf("新客户端加入 IP: %s, 端口: %d\n",
                    inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, ip, sizeof(ip)),
                    ntohs(client_addr.sin_port));

                // 关键步骤：把新生成的 cfd 也挂到 epoll 树上
                // 告诉秘书：以后这个人的消息你也帮我盯着
                ev.events = EPOLLIN;
                ev.data.fd = cfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);

                // 把他记到我们的全局数组里
                add_client(cfd, client_addr);
            }
            // === 情况 B: 如果是其他 fd 响了 ===
            // 说明：已连接的客户端发消息来了
            else
            {
                char buff[1024] = {0};
                // 读取消息
                int len = recv(curfd, buff, sizeof(buff), 0);

                // B.1 读到了数据
                if (len > 0)
                {
                    printf("收到消息: %s\n", buff);
                    
                    // 单独给发消息的人回一句 ACK
                    char *ack = "Server: I got your message!";
                    send(curfd, ack, strlen(ack) + 1, 0);

                    // 给除了他之外的所有人广播
                    broadcast_msg(curfd, buff, len);
                }
                // B.2 读到 0，表示客户端主动断开了连接 (Ctrl+C)
                else if (len == 0)
                {
                    printf("客户端断开连接 (fd=%d)\n", curfd);
                    
                    // 1. 从 epoll 监控中删除 (DEL)
                    epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);
                    // 2. 关闭文件描述符 (挂电话)
                    close(curfd);
                    // 3. 从全局数组移除 (删通讯录)
                    remove_client(curfd);
                }
                // B.3 读到 -1，表示出错了
                else
                {
                    perror("recv");
                    // 出错处理逻辑和断开连接一样：清理掉
                    epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);
                    close(curfd);
                    remove_client(curfd);
                }
            }
        }
    }  
    // 这一步通常走不到，因为上面是死循环
    close(lfd);
    close(epfd);
    return 0;
}