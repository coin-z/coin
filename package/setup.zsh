#!/usr/bin/env zsh

COIN_SHELL=zsh

# source setup.sh from same directory as this file
_COIN_SETUP_DIR=$(builtin cd -q "`dirname "$0"`" > /dev/null && pwd)
emulate -R zsh -c 'source "$_COIN_SETUP_DIR/setup.bash"'
