#!/usr/bin/env bash

nameToCol() {
	case "$1" in
		"black")  :  0;;
		"red")    :  1;;
		"green")  :  2;;
		"yellow") :  3;;
		"blue")   :  4;;
		"purple") :  5;;
		"cyan")   :  6;;
		"white")  :  7;;
		*)        : -1;;
	esac
	echo "$_"
}

fgCode() {
	[ "$1" == "-1" ] || echo -ne "\e[38;5;$1m"
}

bgCode() {
	[ "$1" == "-1" ] || echo -ne "\e[48;5;$1m"
}

die() {
	echo "$1"
	exit 1
}

[ -f "$1" ] || die "File $1 not found!"
[ -f "$1.meta" ] || die "File $1.meta not found!"

texture="$(cat "$1")"
lines="$(wc -l < "$1")"
mask="$(head -n"$lines" "$1.meta")"
IFS=$'\n' read -r -a defs -d '' <<< "$(tail -n+$((lines + 1)) "$1.meta")"
for i in "${defs[@]}"; do
	defkey="$(echo "$i" | awk '{print $1}')"
	# shellcheck disable=SC2034
	def_fg="$(echo "$i" | awk '{print $2}')"
	# shellcheck disable=SC2034
	def_bg="$(echo "$i" | awk '{print $3}')"
	# shellcheck disable=SC2034
	def_fgt="$(echo "$i" | awk '{print $4}')"
	# shellcheck disable=SC2034
	def_bgt="$(echo "$i" | awk '{print $5}')"
	eval "defval_${defkey}_fg=\$def_fg"
	eval "defval_${defkey}_bg=\$def_bg"
	eval "defval_${defkey}_fgt=\$def_fgt"
	eval "defval_${defkey}_bgt=\$def_bgt"
done

current_fg="#"
current_bg="#"

for ((i = 0; i < ${#texture}; i++)); do
	tc="${texture:$i:1}"
	[ "$tc" == $'\n' ] && echo && continue

	mc="${mask:$i:1}"
	name_fg="defval_${mc}_fg"
	name_bg="defval_${mc}_bg"
	next_fg="$(nameToCol "${!name_fg}")"
	next_bg="$(nameToCol "${!name_bg}")"
	[ "$current_fg" != "$next_fg" ] && current_fg="$next_fg" && fgCode "$next_fg"
	[ "$current_bg" != "$next_bg" ] && current_bg="$next_bg" && bgCode "$next_bg"
	echo -n "$tc"
done
