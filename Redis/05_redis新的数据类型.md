bitmap（位图）
操作命令：
SETBIT: 向指定位置(offset)存入一个0或1
GETBIT: 获取指定位置的bit值
BITCOUNT: 统计BitMap中值为1的bit位的数量
BITFIELD: 操作(查询、修改、自增)BitMap中bit数组中的指定位置(offset)的值
BITFIELD_RO: 获取BitMap中bit数组，并以十进制形式返回
BITOP: 将多个BitMap的结果做位运算(与、或、异或)
BITPOS: 查找bit数组中指定范围内第一个0或1出现的位置