# 定义编译器和编译选项，方便以后修改
CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lpthread

# 默认目标（当你只敲 make 时，会执行这个目标）
# 因为我们想同时编译服务端和客户端，所以让它依赖这两个
all: server client

# 编译服务器的规则
server: tcp_server.c
	$(CC) $(CFLAGS) tcp_server.c -o server $(LDFLAGS)

# 编译客户端的规则
client: client.c
	$(CC) $(CFLAGS) client.c -o client

# 清理规则：删除生成的可执行文件
clean:
	rm -f server client