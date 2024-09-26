# Package

## 通过 CMake 构建源码包

```bash
mkdir build && cd build
cmake ..
make -j4
```

## 通过打包命令进行打包

```bash
./tools/package
```

## 打包支持

提供相应的 CMake 函数和宏来支持打包，具体如下：

```cmake
COIN_PACK() # 打包当前项目
```

一个有效的安装包结构为：


- bin：可执行文件目录，存放可执行程序、脚本等
- lib：库目录
- include：头文件目录
- cmake：cmake 脚本目录
- package：包信息目录
- doc：文档目录

