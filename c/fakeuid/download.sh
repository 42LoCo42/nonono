#!/usr/bin/env bash
[ -d "root" ] || ./pacman-root.sh "root"
LD_PRELOAD=./fakeuid.so pacman -Swddr "root" --cachedir "." "$@"
