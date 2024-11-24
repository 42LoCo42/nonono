#!/usr/bin/env bash

isonly() {
	mapfile -t files < <(find "$1" -mindepth 1 -exec basename {} \; | sort -r)
	case "${#files[@]}" in
		1)
			[[ "${files[0]}" =~ .*.c ]] || return 1
		;;

		2)
			[[ "${files[0]}" =~ .*.c ]] || return 1
			[ "${files[1]}" == "Makefile" ] || return 1
		;;

		*)
			return 1
		;;
	esac

	echo "$1"
	mv "$1"/*.c "$1.c"
	rm -f "$1/Makefile"
	rmdir "$1"
}

find . -mindepth 1 -maxdepth 1 -type d \
| while read -r i; do isonly "$i"; done
