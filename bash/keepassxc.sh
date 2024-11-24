#!/usr/bin/env bash
password="$(cat)"
{
	export USER="$PAM_USER"
	export HOME="/home/$USER"
	export DISPLAY=":0"
	export WAYLAND_DISPLAY="wayland-1"
	export XDG_RUNTIME_DIR="/tmp/xdg-runtime-dir-$(id -u "$USER")"
	while ! pgrep -x waybar; do sleep 1; done
	exec chpst -u "$USER" keepassxc --pw-stdin "$HOME/keepass.kdbx" <<< "$password"
} &
