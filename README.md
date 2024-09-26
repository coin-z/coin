# coin
A tool for develop multi node project
coin 是一个可视化多节点管理的工具。可用于快速构建由多个功能节点构成的项目。

通过将复杂系统拆分为单一功能节点，可降低系统的开发难度。本项目正是为此而开发。

## 功能介绍

### Node Manager

### Workshop

### Super Panel


## 构建

### Environment

本项目目前通过 docker 搭建开发环境，通过以下命令构建 docker 镜像，用于本项目的构建。
同时也可以在此容器下进行开发。

```bash
./env/setup.sh
```

镜像构建完成后，可通过下述命令启动容器环境

```bash
./tools/start-env.sh
```

### Build

基础环境构建完成后，通过下述命令执行项目构建

```bash
./env/build.sh
```

### Start up

完成构建后，coin 工具即部署至 docker 容器中，通过下述命令启动

```bash
./env/run.sh
```

### 运行时环境

```bash
sudo apt install libqtermwidget5-0
sudo apt install libc++1
```
