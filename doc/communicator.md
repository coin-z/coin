1. 注释风格采用C/C++风格

    ```C++
    /*
    this is a multi-line comment
    line first
    line second
    */

    // this is a single-line comment
    ```

2. 定义消息体

    1. 基础的消息类型

    |类型名称|说明|
    |-|-|
    |int8|有符号整形|
    |int32|有符号整形|
    |int64|有符号整形|
    |uint8|无符号整形|
    |uint32|无符号整形|
    |uint64|无符号整形|
    |string|字符串|
    |byte|字节|
    |bool|布尔量|

    2. 定义方式

    ```plain
    <type> <name>: <idx> <default-value> @<range>;
    ```

    示例：
    ```plain
    int8 msg: 1 =10 @[-10, 100];
    ```

3. 定义通信子

```plain
communicator <communicator-type> <name-of-communicator> <data-type> <communicator-link>;
```

示例

```text
proto data_comm

message comm
{
    communicator<reader> int reader;
    communicator<writer> int writer;
};
```