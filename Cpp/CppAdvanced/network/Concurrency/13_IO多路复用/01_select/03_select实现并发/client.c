#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main()
{
    // 1. 创建通信的套接字
    // AF_INET: 使用 IPv4 协议
    // SOCK_STREAM: 使用流式协议（TCP）
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1)
    {
        perror("socket error");
        return -1;
    }

    // 2. 连接服务器
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr)); // 初始化内存
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);      // 必须与服务器监听的端口一致
    // 将 IP 地址从字符串转换为网络大端字节序
    inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr.s_addr);

    // 发起连接请求
    int ret = connect(fd, (struct sockaddr*)&saddr, sizeof(saddr));
    if(ret == -1)
    {
        perror("connect error");
        return -1;
    }

    printf("--- 成功连接服务器，请输入内容进行大小写转换 --- \n");
    printf("--- 输入 exit 退出程序 ---\n");

    // 3. 循环通信
    char buff[1024];
    while(1)
    {
        // 提示输入
        printf("insert >> ");
        memset(buff, 0, sizeof(buff));
        
        // 从键盘读取一行输入
        //buff：是读取的文字保存的目的地，sizeof(buff)告诉函数只能读这么大
        //stdin：源头，代表标准输入
        if (fgets(buff, sizeof(buff), stdin) == NULL) {
            break;
        }

        // 去掉末尾的换行符 \n
        buff[strcspn(buff, "\n")] = 0;

        // 如果用户输入 exit，主动断开连接
        if (strcmp(buff, "exit") == 0)
        {
            printf("退出中...\n");
            break;
        }

        // 如果输入为空（只按了回车），则跳过本次循环
        if (strlen(buff) == 0) continue;

        // 发送数据给服务器
        // strlen(buff) + 1 表示把字符串结束符 \0 也发过去，方便服务器直接 printf
        send(fd, buff, strlen(buff) + 1, 0);

        // 接收服务器传回的数据
        memset(buff, 0, sizeof(buff));
        int len = recv(fd, buff, sizeof(buff), 0);
        
        if(len > 0)
        {
            printf("server say: %s\n", buff);
        }
        else if(len == 0)
        {
            printf("服务器端已经断开了连接...\n");
            break;
        }
        else
        {
            perror("recv error");
            break;
        }
    }
    // 4. 关闭文件描述符
    close(fd);
    return 0;
}