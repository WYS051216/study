#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h> // 必须引入线程库，编译时记得加 -lpthread

// === 子线程函数：专门用于接收服务器发来的消息 ===
void *read_msg(void *arg)
{
    // 1. 获取套接字文件描述符
    // arg 是主线程传过来的 fd 的地址，先强转为 int* 再解引用
    int fd = *(int *)arg;
    
    char buff[1024];

    // 2. 进入死循环，一直监听服务器有没有说话
    while(1)
    {
        memset(buff, 0, sizeof(buff)); // 清空缓冲区

        // 3. 阻塞接收数据
        // recv 会停在这里等待，直到服务器发来数据
        int len = recv(fd, buff, sizeof(buff), 0);
        
        if(len > 0)
        {
            // 收到数据，直接打印出来
            printf("%s\n", buff);
        }
        else if(len == 0)
        {
            // recv 返回 0 代表服务器断开了连接 (比如服务器挂了)
            printf("服务器断开连接...\n");
            exit(0); // 退出整个客户端程序
        }
        else
        {
            // recv 返回 -1 代表出错了
            perror("recv");
            exit(-1);
        }
    }
    return NULL;
}

// === 主线程：负责建立连接、启动子线程、处理用户输入 ===
int main(int argc, char *argv[])
{
    // 1. 参数校验：确保用户启动时带上了名字
    // argc 是参数个数，argv 是参数列表
    if(argc < 2)
    {
        printf("启动失败！请指定名字，例如: ./client A\n");
        return -1;
    }
    
    // 获取用户输入的名字 (argv[1])
    char *name = argv[1]; 

    // 2. 创建通信套接字 (买手机)
    // AF_INET: IPv4, SOCK_STREAM: TCP协议
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1)
    {
        perror("socket");
        return -1;
    }

    // 3. 配置服务器的地址信息 (存服务器号码)
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999); // 服务器端口，必须和服务器一致 (9999)
    
    // 将字符串格式的 IP "127.0.0.1" 转换为网络字节序的整数
    inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr.s_addr); 
    
    // 4. 连接服务器 (拨打电话)
    int ret = connect(fd, (struct sockaddr*)&saddr, sizeof(saddr));
    if(ret == -1)
    {
        perror("connect"); // 连接失败报错（比如服务器没开）
        return -1;
    }
    
    printf("成功连接! 你现在的身份是: 客户端%s\n", name);

    // 5. 创建子线程
    // 把 fd 传给子线程，让它去负责“听”消息
    pthread_t tid;
    pthread_create(&tid, NULL, read_msg, &fd);
    
    // 线程分离：子线程结束后自动回收资源，不需要主线程 join 等待
    pthread_detach(tid);

    printf("请开始打字聊天 (输入 exit 退出):\n");

    // 6. 主循环：专门负责处理键盘输入并发送 (说)
    while(1)
    {
        char buff[1024];
        
        // 从键盘读取一行输入 (包含空格)
        // stdin 表示标准输入
        fgets(buff, sizeof(buff), stdin);
        
        // 处理换行符：
        // fgets 会把回车键 '\n' 也读进去，我们用 strcspn 找到 '\n' 并把它替换成结束符 '\0'
        buff[strcspn(buff, "\n")] = '\0';

        // 如果用户直接回车没打字，就跳过不发送
        if(strlen(buff) == 0) continue; 
        
        // 如果输入 exit，则退出聊天
        if(strcmp(buff, "exit") == 0) break;

        // 7. 拼接发送内容
        // 格式：[名字]: 消息内容
        char send_msg[1100]; // 稍微大一点，防止溢出
        sprintf(send_msg, "[%s]: %s", name, buff);
        
        // 8. 发送数据给服务器
        // strlen + 1 是为了把字符串结尾的 '\0' 也发过去
        send(fd, send_msg, strlen(send_msg) + 1, 0);
    }

    // 9. 关闭套接字，释放资源
    close(fd);
    return 0;
}