# 网络编程基础笔记

## 1. 核心概念

### 局域网和广域网
* **局域网 (LAN):** 将一定区域内的各种计算机、外部设备和数据库连接起来形成计算机通信的私有网络。
* **广域网 (WAN):** （外网、公网）连接不同地区局域网或城域网的远程公共网络。

### IP 地址
本质上是一个整形数，用于表示计算机在网络中的地址。有两个主要版本：
* **IPv4:**
    * 使用一个 32 位的整形数描述，占 4 个字节，`int` 型。
    * 常使用点分十进制字符串描述：如 `192.168.247.135`。分成了 4 份，每份 1 个字节，最大值为 255。
    * `0.0.0.0` 是一个特殊的最小 IP 地址（通常指代任意本地地址）。
    * 可用的 IPv4 地址总共有 $2^{32}$ 个。
* **IPv6:**
    * 使用 128 位的整形数描述，占 16 个字节。
    * 使用 16 进制字符串描述：如 `2001:0db8:3c4d:0015:0000:0000:1a2f:1a2b`。分成了 8 份，每份 2 个字节。
    * 可用的 IPv6 地址总共有 $2^{128}$ 个。

### 端口 (Port)
* **作用:** 定位到主机上的一个具体进程。通过这个端口，进程就可以接收到对应的网络数据。
* **数据类型:** 是一个 16 位无符号整形数 `unsigned short`。
* **取值范围:** 有效取值范围是 `0 ~ 65535`。
* **规则:** 如果某个进程不需要网络通信，则不需要绑定端口。一个端口只能给一个进程使用，多个进程不能同时使用同一个端口（有特殊复用情况除外）。

---

## 2. OSI/ISO网络分层模型与协议

### 网络模型
* **OSI 七层模型:** 应用层 -> 表示层 -> 会话层 -> 传输层 -> 网络层 -> 数据链路层 -> 物理层
* **TCP/IP 四层模型:** 应用层 -> 传输层 -> 网络互联层 -> 网络接口层

### 网络协议
计算机网络中互相通信的对等实体之间交换信息时所必须遵守的规则的集合。一般包括五个部分：通信环境、传输服务、词汇表、信息的编码格式、时序规则和过程。

---

## 3. Socket 编程基础

设计者开发了一套接口（相关头文件在 `sys/socket.h` 中），应用程序只需调用这些接口即可完成网络通信。通信主体分为：客户端和服务器端。

### 字节序 (Byte Order)
大于一个字节类型的数据在内存中的存放顺序。单字符和字符串没有字节序问题。
* **主机字节序（小端，Little-Endian）:** 数据的低位字节存储在内存的低地址位，高位字节存储在高地址位。PC 机默认使用小端。
* **网络字节序（大端，Big-Endian）:** 数据的低位字节存储在内存的高地址位。**网络通信中操作的数据（如 IP、端口）必须是大端存储**。

#### 字节序转换函数
```c
// 主机字节序 -> 网络字节序 (Host to Network)
uint16_t htons(uint16_t hostshort);  // 转换短整形 (通常用于端口)
uint32_t htonl(uint32_t hostlong);   // 转换长整形 (通常用于 IP)

// 网络字节序 -> 主机字节序 (Network to Host)
uint16_t ntohs(uint16_t netshort);   // 转换短整形
uint32_t ntohl(uint32_t netlong);    // 转换长整形
```

#### IP 地址转换函数
IP 本质是整数，但常以字符串形式展示。需要进行互转：

**1. 通用转换函数 (支持 IPv4 和 IPv6)**
```c
// 字符串 IP -> 网络字节序整形 IP
int inet_pton(int af, const char *src, void *dst);
/* 参数：
    af: 地址族。AF_INET (IPv4) 或 AF_INET6 (IPv6)
    src: 传入参数，点分十进制的 IP 字符串
    dst: 传出参数，转换得到的大端整形 IP 会写入这块内存
   返回值：成功返回 1，失败返回 0 或 -1
*/

// 网络字节序整形 IP -> 字符串 IP
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
/* 参数：
    af: 地址族
    src: 传入参数，指向存储大端整形 IP 的内存
    dst: 传出参数，存储转换得到的小端字符串 IP
    size: dst 缓冲区的大小，防止越界
   返回值：成功返回指向 dst 的指针，失败返回 NULL
*/
```

**2. 仅支持 IPv4 的转换函数**
```c
// 字符串 IP -> 网络字节序整形 IP
in_addr_t inet_addr(const char *cp);
/* 参数: cp 为传入的十进制字符串 IP
   返回值: 成功返回 32位大端整形数，失败返回 INADDR_NONE (通常是 -1)
*/

// 网络字节序整形 IP -> 字符串 IP
char* inet_ntoa(struct in_addr in);
/* 参数: in 为传入的网络字节序 IP 结构体
   返回值: 返回包含点分十进制 IP 的字符串指针 (注意：内部使用静态缓冲区，非线程安全)
*/
```

---

## 4. TCP 通信流程

TCP 是一个**面向连接的、安全的、流式传输**的传输层协议。
* **面向连接:** 双向连接，通过“三次握手”建立，通过“四次挥手”断开。
* **安全:** 带有数据校验机制，发现数据丢失会自动重传。
* **流式传输:** 发送端和接收端处理数据的速度和数据量可以不一致。

### 服务器端通信流程：
1. `lfd = socket();` // 创建用于监听的套接字
2. `bind();`         // 将监听套接字与本地的 IP 和端口绑定
3. `listen();`       // 设置监听上限，开始监听客户端的连接请求
4. `cfd = accept();` // 阻塞等待连接，成功后得到一个新的通信套接字 (cfd)
5. `read() / write();` // 使用 cfd 进行通信，读写默认阻塞
6. `close();`        // 断开连接，关闭套接字

### 客户端通信流程：
1. `cfd = socket();` // 创建通信套接字
2. `connect();`      // 根据服务器的 IP 和端口发起连接
3. `write() / read();` // 通信
4. `close();`        // 断开连接

---

## 5. 核心套接字函数与结构体详解

### 核心函数

#### 1. `socket` (创建套接字)
```c
int socket(int domain, int type, int protocol);
```
* **domain (地址族):** 指定使用的网络协议族。常用 `AF_INET` (IPv4) 或 `AF_INET6` (IPv6)。
* **type (套接字类型):** 常用 `SOCK_STREAM` (流式协议，对应 TCP) 或 `SOCK_DGRAM` (报文协议，对应 UDP)。
* **protocol (具体协议):** 通常传 `0`，表示根据 type 自动选择默认协议（TCP/UDP）。
* **返回值:** 成功返回有效的文件描述符 (fd)，失败返回 -1。

#### 2. `bind` (绑定IP和端口)
```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```
* **sockfd:** `socket()` 函数返回的监听文件描述符。
* **addr:** 传入参数，指向一个 `sockaddr` 结构体，里面包含要绑定的本地 IP 和 端口信息（必须是网络字节序）。
* **addrlen:** `addr` 指向的结构体的内存大小（使用 `sizeof` 计算）。
* **返回值:** 成功返回 0，失败返回 -1。

#### 3. `listen` (设置监听)
```c
int listen(int sockfd, int backlog);
```
* **sockfd:** 监听的文件描述符。
* **backlog:** 排队连接队列的最大长度（即同时允许有多少个客户端处于三次握手建立连接的排队状态）。通常设为 128 或更大。
* **返回值:** 成功返回 0，失败返回 -1。

#### 4. `accept` (接受连接)
```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
* **sockfd:** 监听的文件描述符。
* **addr:** 传出参数，用于保存**成功连接的那个客户端**的 IP 和端口信息。如果不关心客户端信息，可以传 `NULL`。
* **addrlen:** 传入传出参数，记录 `addr` 结构体的大小。如果不关心，传 `NULL`。
* **返回值:** 成功返回一个新的文件描述符（专门用于和这个客户端进行通信），失败返回 -1。

### 关键地址结构体

```c
// 1. 通用地址结构体 (bind, accept 等函数的参数类型)
struct sockaddr {
    sa_family_t sa_family;  // 地址族协议 (如 AF_INET)，2字节
    char sa_data[14];       // 端口(2字节) + IP地址(4字节) + 填充(8字节)
};

// --- 下面是具体的底层类型定义 ---
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef uint16_t in_port_t;
typedef uint32_t in_addr_t;
typedef unsigned short int sa_family_t;

// 2. IP 地址结构体
struct in_addr {
    in_addr_t s_addr;       // 32 位网络字节序的整形 IP 地址
};

// 3. IPv4 专用地址结构体 (在实际编程中主要操作这个结构体，然后强转为 sockaddr)
struct sockaddr_in {
    sa_family_t sin_family; // 地址族协议 (AF_INET)
    in_port_t sin_port;     // 端口，2字节 (必须使用 htons 转换)
    struct in_addr sin_addr;// IP地址，4字节 (必须是网络字节序)
    
    // 填充 8 字节，为了和 struct sockaddr 保持总大小一致 (16字节)
    unsigned char sin_zero[sizeof(struct sockaddr) - sizeof(sa_family_t) - sizeof(in_port_t) - sizeof(struct in_addr)];
};
```

#### 5. `connect` (连接服务器)
```c
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```
* **sockfd:** 客户端创建的用于通信的文件描述符。
* **addr:** 传入参数，指向**服务器**的 IP 和端口信息结构体（客户端需要主动连接的对象）。
* **addrlen:** 服务器地址结构体 `addr` 的内存大小。
* **返回值:** * 成功返回 0。
    * 失败返回 -1，并设置 `errno`。

#### 6. `close` (关闭套接字)
```c
int close(int fd);
```
* **fd:** 需要关闭的文件描述符（可以是服务器的监听 `lfd`，也可以是通信用的 `cfd`）。
* **返回值:** * 成功返回 0。
    * 失败返回 -1。关闭后，该文件描述符不再指向任何打开的文件/套接字，资源被系统回收。

#### 7. `recv` (接收数据)
```c
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```
* **sockfd:** 用于通信的文件描述符（通常是 `accept` 或 `socket` 产生的）。
* **buf:** 指向一块内存缓冲区，用于存放从网络读到的数据。
* **len:** 缓冲区 `buf` 的最大容量。
* **flags:** 指定接收方式。
    * 常用 `0`: 阻塞接收，直到有数据到达。
    * `MSG_PEEK`: 查看数据但不从系统缓冲区清除。
    * `MSG_WAITALL`: 尽量等待直到满足 `len` 长度的数据全部到达。
* **返回值:**
    * **大于 0:** 实际接收到的字节数。
    * **等于 0:** 表示对方（对等端）已经关闭了连接。在 TCP 通信中，这是判断客户端或服务器退出的重要依据。
    * **等于 -1:** 接收失败。如果 `errno` 为 `EAGAIN` 或 `EWOULDBLOCK`，表示当前没有数据（非阻塞模式）；其他值表示发生了真正的错误。

### 关键地址结构体

```c
// 1. 通用地址结构体 (bind, accept, connect 等函数的参数类型)
struct sockaddr {
    sa_family_t sa_family;  // 地址族协议 (如 AF_INET)，2字节
    char sa_data[14];       // 端口(2字节) + IP地址(4字节) + 填充(8字节)
};

// --- 下面是具体的底层类型定义 ---
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef uint16_t in_port_t;
typedef uint32_t in_addr_t;
typedef unsigned short int sa_family_t;

// 2. IP 地址结构体
struct in_addr {
    in_addr_t s_addr;       // 32 位网络字节序的整形 IP 地址
};

// 3. IPv4 专用地址结构体 (在实际编程中主要操作这个结构体，然后强转为 sockaddr)
struct sockaddr_in {
    sa_family_t sin_family; // 地址族协议 (AF_INET)
    in_port_t sin_port;     // 端口，2字节 (必须使用 htons 转换)
    struct in_addr sin_addr;// IP地址，4字节 (必须是网络字节序)
    
    // 填充 8 字节，为了和 struct sockaddr 保持总大小一致 (16字节)
    unsigned char sin_zero[sizeof(struct sockaddr) - sizeof(sa_family_t) - sizeof(in_port_t) - sizeof(struct in_addr)];
};
```