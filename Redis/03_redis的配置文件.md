# Redis 核心配置文件 (redis.conf) 详解笔记

## 一、 单位配置 (Units)
Redis 对配置的容量单位有严格的大小写区分和进制规定（**只支持 bytes，不支持 bit**，并且对大小写不敏感）：
* `1k` => 1000 bytes
* `1kb` => 1024 bytes
* `1m` => 1000000 bytes
* `1mb` => 1024*1024 bytes
* `1g` => 1000000000 bytes
* `1gb` => 1024*1024*1024 bytes

---

## 二、 包含配置 (INCLUDES)
* **`include`**：可以通过此指令将其他额外的配置文件包含进来，通常用于多实例环境下的公共配置抽取。

---

## 三、 网络配置 (NETWORK)

* **`bind`**
  * 默认情况：`bind 127.0.0.1 -::1` （只能接受本机的访问请求）。
  * 生产环境：必须将其修改为具体的内网 IP，或者注释掉/改为 `0.0.0.0` 以允许无限制接收任何 IP 地址的访问。
* **`protected-mode`**
  * 保护模式。如果开启（`yes`），且没有设定 `bind` 也没有设置密码（`requirepass`），Redis 只允许接受本机的响应，拒绝所有外部连接。
* **`port`**
  * Redis 的默认监听端口号：`6379`。
* **`tcp-backlog`**
  * 设置 TCP 的 backlog，它是一个连接队列（backlog 队列总和 = 未完成三次握手队列 + 已完成三次握手队列）。
  * 在高并发环境下，需要一个高 backlog 值来避免慢客户端连接问题。
* **`timeout`**
  * 设置一个空闲的客户端维持多少秒后会自动关闭连接。
  * `0` 表示关闭该功能，即永不主动关闭空闲连接。
* **`tcp-keepalive`**
  * 对访问客户端的一种心跳检测，每隔 N 秒检测一次。
  * 单位为秒，默认值为 `300`。如果设置为 `0`，则不会进行 TCP 层面的心跳检测。

---

## 四、 通用配置 (GENERAL)

* **`daemonize`**
  * 是否为后台进程，设置为 `yes` 时表示以守护进程（后台）方式启动。
  * **⚠️ 实战高危提示：** 如果在 Docker 容器中运行 Redis，**必须保持 `daemonize no`**。因为 Docker 的生命周期依赖于前台主进程，如果让 Redis 潜入后台，Docker 容器会瞬间判定任务结束并自动退出（挂起）。
* **`pidfile`**
  * 当 Redis 以守护进程运行时，它会把进程号（PID）写入指定的 pidfile 文件中。
* **`loglevel`**
  * 日志级别，支持四个等级：
    1. `debug`（记录大量信息，适用于开发测试）
    2. `verbose`（记录许多有用的信息）
    3. `notice`（**默认值**，记录生产环境适用的适度信息）
    4. `warning`（仅记录非常重要/关键的警告信息）
* **`logfile`**
  * 指定日志文件的名称和路径。如果为空字符串，则将日志输出到标准输出。
* **`databases`**
  * 设定数据库的数量。默认为 `16` 个（编号 0 - 15）。
  * 默认连接的是 `0` 号数据库，可以使用 `SELECT <dbid>` 命令在连接上切换数据库。

---

## 五、 安全配置 (SECURITY)

* **`requirepass`**
  * 设置 Redis 连接密码。开启后，客户端需要使用 `AUTH <password>` 命令进行身份验证。

---

## 六、 资源限制 (LIMITS)

* **`maxclients`**
  * 设置 Redis 同时可以与多少个客户端进行连接，默认情况下为 `10000` 个客户端。
  * 如果达到了此限制，Redis 则会拒绝新的连接请求，并向请求方发出 `max number of clients reached` 的错误回应。
* **`maxmemory`**
  * 设置 Redis 可以使用的最大内存量（单位 bytes）。
  * 一旦到达内存使用上限，Redis 将会试图根据 `maxmemory-policy` 指定的规则移除内部数据。
  * 如果无法移除数据，或者设置了“不允许移除”，Redis 会针对申请内存的写指令（如 `SET`、`LPUSH` 等）返回错误信息，但读指令（如 `GET`）仍可正常执行。

### 🔥 核心补充：maxmemory-policy (八大内存淘汰策略)
当内存达到 `maxmemory` 上限时，Redis 支持以下 8 种淘汰策略：

1. **`noeviction`**（默认策略）：不驱逐任何数据，拒绝所有写入操作并返回客户端错误信息（OOM）。
2. **`allkeys-lru`**：对**所有 Key** 使用 LRU 算法（Least Recently Used，淘汰最近最少使用的 Key）。*(最常用的策略)*
3. **`volatile-lru`**：仅对**设置了过期时间的 Key** 使用 LRU 算法淘汰。
4. **`allkeys-random`**：对**所有 Key** 随机淘汰。
5. **`volatile-random`**：仅对**设置了过期时间的 Key** 随机淘汰。
6. **`volatile-ttl`**：仅对**设置了过期时间的 Key** 进行淘汰，越早过期（TTL 越小）的优先淘汰。
7. **`allkeys-lfu`**：对**所有 Key** 使用 LFU 算法（Least Frequently Used，淘汰使用频率最少的 Key）。*(Redis 4.0 引入)*
8. **`volatile-lfu`**：仅对**设置了过期时间的 Key** 使用 LFU 算法淘汰。*(Redis 4.0 引入)*