#!/usr/bin/env bash

generic_log() {
	printf "[1m[%s%s[38;5;15m] [%s] [%s] %s%s[m\n" \
		"$1" "$2" "$steptext/$total_steps" "$(date +"%a %b %d %T %Z %Y")" "$1" "$3"
}

work() {
	generic_log "[38;5;4m" "WORK" "$1"
}

info() {
	generic_log "[38;5;2m" "INFO" "$1"
}

warn() {
	generic_log "[38;5;3m" "WARN" "$1"
}

crit() {
	generic_log "[38;5;1m" "CRIT" "$1"
}

die() {
	crit "Failure at step $current_step"
	echo "$current_step" > /tmp/steplast
	exit 1
}

fakeinstall_all() {
	for p in /var/lib/pacman/local/*; do
		fakeinstall "$p" &
	done
	wait
}

fakeinstall() {
	mkdir -p "/tmp$1"
	for f in "$1"/*; do
		ln -s "$f" "/tmp$f" &
	done
	wait
}

# prepare some variables
total_steps="$(
	grep -c "^step " "${BASH_SOURCE[0]}"
)"
fmt="%$(echo -n "$total_steps" | wc -c)d"
current_step=0
redir=0

step() {
	((current_step = current_step + 1))
	((current_step < target_step)) && return
	desc="$1"
	cmd="$2"
	shift 2
	# shellcheck disable=SC2059
	# the format string is a variable
	steptext="$(printf "$fmt" "$current_step")"
	work "$desc"
	if ((redir == 0)); then
		$cmd "$@" || die
	else
		$cmd "$@" 1>/tmp/stepout 2>/tmp/steperr || die
	fi
}

step "Loading..." true
target_step="${1:-0}"

[ -f "/tmp/steplast" ] && {
	last_step="$(< "/tmp/steplast")"
	info "A saved step was found (Step $last_step)"
	warn "Do you want to resume from there?"
	read -r -p "[y/n] > " resume_last_step
	case "$resume_last_step"
		in y*) target_step="$last_step" ;;
	esac
}

step "Make pacman prettier" sed -Ei \
	-e 's|^#Color$|Color|g' \
	-e 's|^#TotalDownload$|TotalDownload\nILoveCandy|g' \
	"/etc/pacman.conf"
step "Prepare pacman env in ramdisk" mkdir -p "/tmp/var/lib/pacman"
step "Get the databases" pacman -r "/tmp" -Sy
step "Fake install all current packages" fakeinstall_all
step "Get gcc" pacman -r "/tmp" -S --noconfirm gcc
step "Savepoint" false
