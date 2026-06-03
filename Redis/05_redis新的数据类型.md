### 1. Bitmap (位图)
**概念**：Bitmap 并不是一种独立的数据类型，它底层实际上是 String 类型，但它提供了一套命令，允许我们直接对字符串的二进制位（bit）进行操作。非常适合用于状态统计（如签到、在线状态等），极其节省内存。

**命令操作与完整参数**：

* **`SETBIT key offset value`**
    * **作用**：向指定位置 (`offset`) 存入一个 0 或 1。
    * **参数**：`offset` (偏移量，从 0 开始)，`value` (只能是 0 或 1)。
* **`GETBIT key offset`**
    * **作用**：获取指定位置的 bit 值。返回 0 或 1。
    * **参数**：`offset` (偏移量)。
* **`BITCOUNT key [start end [BYTE | BIT]]`**
    * **作用**：统计 BitMap 中值为 1 的 bit 位的数量。
    * **参数**：`start` 和 `end` 是可选的，用于指定范围。后面的可选参数决定了范围是按字节(`BYTE`)算还是按位(`BIT`)算（Redis 7.0.0 引入）。
* **`BITFIELD key [GET type offset] [SET type offset value] [INCRBY type offset increment] [OVERFLOW WRAP|SAT|FAIL]`**
    * **作用**：将 Bitmap 视为由多个整数组成的数组，可以对指定位置的位域进行查询、修改、自增操作。
    * **参数**：
        * `type`：被操作的整数类型，如 `i8` (8位有符号整数), `u4` (4位无符号整数)。
        * `offset`：从哪个位开始。
        * `value` / `increment`：设置的值或增加的步长。
        * `OVERFLOW`：溢出控制（WRAP 折返，SAT 饱和，FAIL 失败）。
* **`BITFIELD_RO key [GET type offset]`**
    * **作用**：`BITFIELD` 的只读版本，获取 BitMap 中 bit 数组的值，并以十进制形式返回。只能使用 `GET` 子命令。
* **`BITPOS key bit [start [end [BYTE | BIT]]]`**
    * **作用**：查找 bit 数组中指定范围内，第一个出现 0 或 1 的位置（偏移量）。
    * **参数**：`bit` (要找的 0 或 1)，`start` 和 `end` (可选，指定字节范围或位范围)。

* **`BITOP operation destkey key [key ...]`**
    * **作用**：将多个 BitMap 的结果做逻辑位运算（与、或、异或、非），并将结果保存在 `destkey` 中。
    * **参数**：`operation` (可以是 `AND`, `OR`, `XOR`, `NOT`)，`destkey` (保存结果的目标键)，`key` (参与运算的源键)。
    * **示例代码（统计连续两天都签到的用户）：**
        ```redis
        # 假设 bitmap 的 offset 代表用户ID，1 代表已签到
        # day1: 用户 1, 2, 5 签到了
        127.0.0.1:6379> SETBIT sign:day1 1 1
        127.0.0.1:6379> SETBIT sign:day1 2 1
        127.0.0.1:6379> SETBIT sign:day1 5 1
        
        # day2: 用户 2, 5, 8 签到了
        127.0.0.1:6379> SETBIT sign:day2 2 1
        127.0.0.1:6379> SETBIT sign:day2 5 1
        127.0.0.1:6379> SETBIT sign:day2 8 1
        
        # 使用 BITOP 执行 AND (与) 运算，结果存入 sign:both_days
        127.0.0.1:6379> BITOP AND sign:both_days sign:day1 sign:day2
        (integer) 1
        
        # 此时 sign:both_days 中为 1 的 offset 就是连续签到的用户 (ID 2 和 5)
        127.0.0.1:6379> BITCOUNT sign:both_days
        (integer) 2
        ```

---

### 2. HyperLogLog
**基数问题**：求集合中**不重复**的元素的个数的问题。
**概念**：HyperLogLog 是用来做基数统计的算法。它的优点是，在输入元素的数量或者体积非常非常大时，计算基数所需的空间总量是固定的，并且是很小的。
在 Redis 里面，每个 HyperLogLog 键只需要花费 12KB 内存，就可以计算接近 2^64 个不同元素的基数。这和集合形成鲜明对比（集合元素越多，耗费内存就越大）。
**注意**：因为 HyperLogLog 只会根据输入元素来计算基数，而不会存储输入元素本身，所以它**不能**像集合那样返回输入的各个元素，且存在约 0.81% 的标准误差（属于概率算法）。

**命令操作与完整参数**：

* **`PFADD key element [element ...]`**
    * **作用**：将指定元素添加到 HyperLogLog 中。
    * **参数**：`key` (键名)，`element` (一个或多个要添加的元素)。
* **`PFCOUNT key [key ...]`**
    * **作用**：返回给定 HyperLogLog 的基数估算值（如果不止一个 key，则返回合并后的基数估算值）。
    * **参数**：可以传入一个或多个 `key`。
* **`PFMERGE destkey sourcekey [sourcekey ...]`**
    * **作用**：将多个 HyperLogLog 合并为一个 HyperLogLog。
    * **参数**：`destkey` (合并后存入的目标键)，`sourcekey` (一个或多个被合并的源键)。

---

### 3. Geospatial (地理空间)
**概念**：该类型用于存储元素的二维坐标，在地图上就是经纬度。Redis 基于该类型，提供了经纬度设置、查询、距离查询、范围查询等常见操作。底层实际上是通过 Geohash 算法将经纬度转换为 52 位整数，并利用 ZSET（有序集合）进行存储。

**命令操作与完整参数**：

* **`GEOADD key [NX|XX] [CH] longitude latitude member [longitude latitude member ...]`**
    * **作用**：将给定的空间元素（经度、纬度、名字）添加到指定的键里面。
    * **参数**：`longitude` (经度，必须在纬度前面！范围 -180 到 180)，`latitude` (纬度，范围 -85.05112878 到 85.05112878)，`member` (地点的名称/标识)。
* **`GEOPOS key member [member ...]`**
    * **作用**：从键里面返回所有给定位置元素的位置（经度和纬度）。
    * **参数**：`key`，以及一个或多个 `member` 名称。
* **`GEODIST key member1 member2 [M | KM | FT | MI]`**
    * **作用**：返回两个给定位置之间的距离。
    * **参数**：`member1` 和 `member2` (两个已存在的地点)。最后的可选参数是单位：`M` (米，默认), `KM` (千米), `FT` (英尺), `MI` (英里)。
* **`GEORADIUS key longitude latitude radius M | KM | FT | MI [WITHCOORD] [WITHDIST] [WITHHASH] [COUNT count [ANY]] [ASC|DESC] [STORE key] [STOREDIST key]`**
    * *(注：在 Redis 6.2 版本之后，官方推荐使用 `GEOSEARCH` 代替此命令，但了解它依然很有用)*
    * **作用**：以给定的经纬度为中心，返回键包含的位置元素当中，与中心的距离不超过给定最大半径的所有位置元素（即“搜索附近的人/店”）。
    * **参数**：
        * `longitude latitude`：中心点的经纬度。
        * `radius`：搜索半径。
        * `M | KM | FT | MI`：半径的单位。
        * `WITHCOORD`：将符合条件的经纬度也一并返回。
        * `WITHDIST`：将符合条件的位置与中心的距离也一并返回。
        * `ASC|DESC`：根据距离按照由近到远（ASC）或由远到近（DESC）排序。
        * `COUNT count`：限制返回的结果数量。