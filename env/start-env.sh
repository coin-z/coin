#!/bin/env bash

# 获取脚本所在位置
arg0=${BASH_SOURCE[0]}
if [ ! $arg0 ];
then
    arg0=$0
fi
SCRIPT_PATH="$(dirname "$(realpath "${arg0}")")"
echo "environment path: ${SCRIPT_PATH}"

CONTAINER_NAME_OR_ID=coin
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

# 检查容器是否存在
if docker ps -a --format '{{.Names}}' | grep -q "^${CONTAINER_NAME_OR_ID}\$"; then
  echo "Container '$CONTAINER_NAME_OR_ID' already exists."
else
  echo "Container '$CONTAINER_NAME_OR_ID' not exists. Start it..."
  start_container
fi

echo "Done."
