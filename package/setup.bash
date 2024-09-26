#!/usr/bin/env bash

# 计算脚本所在位置
# 获取当前脚本所在目录
arg0=${BASH_SOURCE[0]}
if [ ! $arg0 ];
then
    arg0=$0
fi

SCRIPT_PATH="$(dirname "$(realpath "${arg0}")")"


# 计算当前脚本所在位置的绝对路径
CDIR=$(realpath "$SCRIPT_PATH")

unset SCRIPT_PATH

# echo "current dir: ${CDIR}"

# 将 bin 目录添加到系统查找项
export PATH=${CDIR}/bin:$PATH

# 将 lib 目录添加到系统查找项
export LD_LIBRARY_PATH=${CDIR}/lib:$LD_LIBRARY_PATH

# 将 pkgconfig 目录添加到系统查找项
export PKG_CONFIG_PATH=${CDIR}/lib/pkgconfig:$PKG_CONFIG_PATH

# 设置 root 目录环境变量
export COIN_ROOT=${CDIR}
export Coin_ROOT=${COIN_ROOT}

unset CDIR
