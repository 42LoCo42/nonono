#!/usr/bin/env bash

ANSI_ESCAPE="$(printf "\u1b")"
ANSI_RESET="$(printf "\u1b[m")"
ANSI_CUR_HOME="$(printf "\u1b[H")"
ANSI_CUR_UP="$(printf "\u1b[A")"
ANSI_CUR_DOWN="$(printf "\u1b[B")"
ANSI_CUR_RIGHT="$(printf "\u1b[C")"
ANSI_CUR_LEFT="$(printf "\u1b[D")"
ANSI_CUR_SAVE="$(printf "\u1b[s")"
ANSI_CUR_LOAD="$(printf "\u1b[u")"
ANSI_ERASE="$(printf "\u1b[2K")"

texture_height="12"
texture_width="38"

getCursorPos() {
	IFS='[;' read -p $'\e[6n' -d R -rs _ y x _
}

toPromptArea() {
	echo -ne "${ANSI_CUR_SAVE}\e[$((texture_height + 1));0H${ANSI_ERASE}"
}

toTexture() {
	echo -n "${ANSI_CUR_LOAD}"
}

clear
echo -n "${ANSI_CUR_SAVE}${ANSI_CUR_HOME}"
# displayTexture
echo -n "$ANSI_CUR_LOAD"
while true; do
	getCursorPos
	toPromptArea
	printf '%2s' "$x"
	echo -n " "
	printf '%2s' "$y"
	toTexture
	read -rsn1 input
	[ "$input" == "$ANSI_ESCAPE" ] && read -rsn2 input
	case "$input" in
		"[A") [ "$y" == 1 ] || echo -n "$ANSI_CUR_UP";;
		"[B") [ "$y" == "$texture_height" ] || echo -n "$ANSI_CUR_DOWN";;
		"[C") [ "$x" == "$texture_width" ] || echo -n "$ANSI_CUR_RIGHT";;
		"[D") [ "$x" == 1 ] || echo -n "$ANSI_CUR_LEFT";;
	esac
done
