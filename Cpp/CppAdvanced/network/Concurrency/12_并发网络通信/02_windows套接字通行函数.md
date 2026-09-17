初始化套接字环境
    使用windows中的套接字函数需要额外包含对应的头文件以及加载相应的动态库
    include<winsock2.h>
    ws2_32.dll
    加载套接字库的函数
        WSAStartup(WORD wVersionRequested, LPWSASATA lpWSAData);
        将里面的参数以及返回值补充说明一下
    套接字通信完成之后需要关掉套接字库
        int WSAClearup(void)
        将里面的参数以及返回值补充说明一下
    使用举例：
        WSAData wsa;
        //初始化套接字库
        WSAStartup(MAKEWORD(2, 2), &wsa)
        //.....
        //注释winsock相关库
        WSAClearup();

套接字通信函数
    结构体
        typedef struct in_addr{
            union{
                struct{unsigned char s_b1, s_b2, s_b3, s_b4;} S_un_b;
                struct{ unsigned short s_w1, s_w2;} S_un_w;
                unsigned long S_addr;   //存储IP地址
            } S_un;
        } IN_ADDR;

        struct sockaddr_in{
            short int sin_family;
            unsigned short int sin_port;
            struct in_addr sin_addr;
            unsigned char sin_zero[8];
        }
    大小端转化函数
        主机字节序转化为网络字节序
        u_short htons(u_short hostshort);
        u_long htonl(u_long hostlong);
        网络字节序转化为主机字节序
        u_short ntohs(u_short netshort);
        u_long ntohl(u_long netlong);
        //点分十进制转化为大端整形,处理ipv4的IP地址
        unsigned long inet_addr(const char FAR * cp)
        //大端整形转化为点分十进制IP
        char* inet_ntoa(struct in_addr in);
    套接字函数
        //创建一个套接字
        SOCKET socket(int af, int type; int protocal);
        将里面的参数和返回值补充完整
        //套接字绑定本地的ip和端口
        int bind(SOCKET s, const struct sockaddr FAR* name, int namelen);
        将里面的参数和返回值补充完整
        //设置监听
        int listen(SOCKET s, int backlong);
        将里面的参数和返回值补充完整
        //等待并接收客户端连接
        SOCKET accept(SOCKET s, struct sockaddr FAR* addr, int FAR* addrlen);
        将里面的参数和返回值补充完整
        //接收数据
        int recv(SOCKET s, char FAR* buf, int flags);
        将里面的参数和返回值补充完整
        //发送数据
        int send(SOCKET s, char FAR* buf, int len, int flags);
        将里面的参数和返回值补充完整
        //关闭套接字
        int closesocket(SOCKET s);
        将里面的参数和返回值补充完整

文件描述符在套接字通信中的作用：
    文件描述符对应的内存结构：
        一个文件描述符对应两块内存，一块内存是读缓冲区，一块内存是写缓冲区
        读数据：通过文件描述符将内存中的数据读出，这块内存称之为读缓冲区
        写数据：通过文件描述符将数据写入到某块内存中，这块内存可以称之为写缓冲区