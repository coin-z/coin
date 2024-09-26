#!/bin/env bash
CONTAINER_NAME_OR_ID=coin
IMAGE_NAME=coin
IMAGE_TAG=20.04

# 获取当前脚本所在目录
arg0=${BASH_SOURCE[0]}
if [ ! $arg0 ];
then
    arg0=$0
fi
SCRIPT_PATH="$(dirname "$(realpath "${arg0}")")"
env_root=${SCRIPT_PATH}/coin-env-root

function start_container() {
    docker run -itd --rm --net=host -e DISPLAY=:1 -v /tmp/.X11-unix:/tmp/.X11-unix \
        -v $SCRIPT_PATH/..:/home/coin/coin \
        -v $env_root/workspace:/home/coin/workspace \
        -v $env_root/coin:/opt/coin \
        -v $env_root/config:/home/coin/.config/coin \
        -v $env_root/vscode/.vscode:/home/coin/.vscode \
        -v $env_root/vscode/.vscode-remote-containers:/home/coin/.vscode-remote-containers \
        -v $env_root/vscode/.vscode-server:/home/coin/.vscode-server \
        -v $env_root/cache:/home/coin/.cache \
        --name coin \
        coin:20.04 $@
}

# 检查 environment 是否存在
if [ ! -d $env_root ]; then
    echo "Environment '$env_root' does not exist, create it."
    mkdir -p $env_root
    sudo chown $USER:$USER $env_root
fi

# 检查工作目录是否存在，不存在则创建
if [ ! -d $env_root/workspace ]; then
    echo "Workspace '$env_root/workspace' does not exist, create it."
    mkdir -p $env_root/workspace
    sudo chown $USER:$USER $env_root/workspace
fi

# 检查配置文件目录是否存在，不存在则创建
if [ ! -d $env_root/config ]; then
    echo "Config '$env_root/config' does not exist, create it."
    mkdir -p $env_root/config
    sudo chown $USER:$USER $env_root/config
fi

# 检查安装文件目录是否存在，不存在则创建
if [ ! -d $env_root/coin ]; then
    echo "Coin '$env_root/coin' does not exist, create it."
    mkdir -p $env_root/coin
    sudo chown $USER:$USER $env_root/coin
fi

# 检查 vscode 文件目录是否存在，不存在则创建
if [ ! -d $env_root/vscode ]; then
    echo "VSCODE '$env_root/vscode' does not exist, create it."
    mkdir -p $env_root/vscode
    mkdir $env_root/vscode/.vscode
    mkdir $env_root/vscode/.vscode-remote-containers
    mkdir $env_root/vscode/.vscode-server
    sudo chown -R $USER:$USER $env_root/vscode
fi

# 检查 cache 文件目录是否存在，不存在则创建
if [ ! -d $env_root/cache ]; then
    echo "CACHE '$env_root/cache' does not exist, create it."
    mkdir -p $env_root/cache
    sudo chown $USER:$USER $env_root/cache
fi

# 检查镜像是否存在
if docker images | grep -q "${IMAGE_NAME}\s*${IMAGE_TAG}"; then
    echo "Image ${IMAGE_NAME}:${IMAGE_TAG} already exists. skip build."
else
    echo "Image ${IMAGE_NAME}:${IMAGE_TAG} Not Exists. Build it..."
    cd ${SCRIPT_PATH}/coin-ubuntu-20.04
    docker build -t ${IMAGE_NAME}:${IMAGE_TAG} .
    
    if [ $? -eq 0 ]; then
        echo "Image ${IMAGE_NAME}:${IMAGE_TAG} Build Done."
    else
        echo "Image ${IMAGE_NAME}:${IMAGE_TAG} Build FAILED."
        exit 1
    fi
fi

# 检查容器是否存在
if docker ps -a --format '{{.Names}}' | grep -q "^${CONTAINER_NAME_OR_ID}\$"; then
    echo "Container '$CONTAINER_NAME_OR_ID' already exists."
else
    echo "Container '$CONTAINER_NAME_OR_ID' not exists. Start it..."
    start_container
sleep 1
fi

# 检查 setup.bash 是否存在
if [ ! -f $env_root/coin/hydrogen/setup.bash ]; then
    echo "File '$env_root/setup.bash' does not exist, setup it."
    docker exec -it coin /bin/zsh -c "ls /opt/coin && cd /home/coin/coin && ./tools/set_env"
fi

# 检查 kitbag 是否存在
# if [ ! -d $env_root/kitbag ]; then
    # echo "Kitbag '$env_root/kitbag' does not exist, install it."
    docker exec -it coin /bin/zsh -c "source /opt/coin/hydrogen/setup.zsh && cd /home/coin/coin/tools && ./kit clone"
# fi

# 检查 kit 是否存在，如不存在则进行构建
if [ ! -d $env_root/coin/hydrogen/kit ]; then
    echo "Kit '$env_root/coin/hydrogen/kit' does not exist, build it."
    docker exec -it coin /bin/zsh -c "source /opt/coin/hydrogen/setup.zsh && cd /home/coin/coin/tools  && ./kit clean && ./kit build"
fi
