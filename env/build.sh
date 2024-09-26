#!/bin/env bash

# 获取脚本所在位置
arg0=${BASH_SOURCE[0]}
if [ ! $arg0 ];
then
    arg0=$0
fi
SCRIPT_PATH="$(dirname "$(realpath "${arg0}")")"
echo "environment path: ${SCRIPT_PATH}"

${SCRIPT_PATH}/start-env.sh

echo "Start build..."
docker exec -it coin /bin/zsh -c "source /opt/coin/hydrogen/setup.zsh && cd /home/coin/coin && ./tools/package"
echo "Done."
