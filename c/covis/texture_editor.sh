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

die() {
	echo "$1"
	exit 1
}

texture_file="$1"
[ -f "$texture_file" ] || die "File $texture_file not found!"
IFS=$'\n' read -r -a texture_lines -d '' <<< "$(cat "$texture_file")"
texture_height="$(wc -l < "$texture_file")"

# pad texture file with spaces
rm -f "_$texture_file"
# shellcheck disable=SC2000
texture_width="$(for i in "${texture_lines[@]}"; do echo "$i" | wc -c; done | sort -gr | head -n1)"
texture_width=$((texture_width - 1))
for i in "${texture_lines[@]}"; do
	printf "%-${texture_width}s" "$i" >> "_$texture_file"
	echo >> "_$texture_file"
done

# reload texture
mv "_$texture_file" "$texture_file"
IFS=$'\n' read -r -a texture_lines -d '' <<< "$(cat "$texture_file")"

# sequence storage arrays
declare -a fg_seqs
declare -a bg_seqs
fg_seqs[0]="0 0 \e[38;5;1m"
bg_seqs[0]="0 0 \e[48;5;0m"
bg_seqs[1]="1 0 \e[48;5;1m"
bg_seqs[2]="2 0 \e[48;5;2m"
bg_seqs[3]="3 0 \e[48;5;3m"
bg_seqs[4]="4 0 \e[48;5;4m"
bg_seqs[5]="5 0 \e[48;5;5m"
bg_seqs[6]="6 0 \e[48;5;6m"
bg_seqs[7]="7 0 \e[48;5;7m"
bg_seqs[8]="8 0 $ANSI_RESET"

nextFgCoords() {
	next_fg_x="$(echo "${fg_seqs[$((current_fg_seq + 1))]}" | awk '{print $1}')"
	next_fg_y="$(echo "${fg_seqs[$((current_fg_seq + 1))]}" | awk '{print $2}')"
}
nextBgCoords() {
	next_bg_x="$(echo "${bg_seqs[$((current_bg_seq + 1))]}" | awk '{print $1}')"
	next_bg_y="$(echo "${bg_seqs[$((current_bg_seq + 1))]}" | awk '{print $2}')"
}
getCurrentFgSeq() {
	echo "${fg_seqs[${current_fg_seq}]}" | awk '{print $3}'
}
getCurrentBgSeq() {
	echo "${bg_seqs[${current_bg_seq}]}" | awk '{print $3}'
}

displayTexture() {
	local current_fg_seq=-1
	local current_bg_seq=-1
	local emit_seq=""
	nextFgCoords
	nextBgCoords
	for ((y = 0; y < "$texture_height"; y++)); do
		current_line="${texture_lines[${y}]}"
		for ((x = 0; x < "$texture_width"; x++)); do
			[ "$x" == "$next_fg_x" ] && [ "$y" == "$next_fg_y" ] &&
				current_fg_seq=$((current_fg_seq + 1)) && nextFgCoords &&
				emit_seq="$(getCurrentFgSeq)" && echo -ne "$emit_seq" &&
				[ "$emit_seq" == "$ANSI_RESET" ] && echo -ne "$(getCurrentBgSeq)"
			[ "$x" == "$next_bg_x" ] && [ "$y" == "$next_bg_y" ] &&
				current_bg_seq=$((current_bg_seq + 1)) && nextBgCoords &&
				emit_seq="$(getCurrentBgSeq)" && echo -ne "$emit_seq" &&
				[ "$emit_seq" == "$ANSI_RESET" ] && echo -ne "$(getCurrentFgSeq)"
			echo -ne "${current_line:$x:1}"
		done
		echo
	done
}

getCursorPos() {
	IFS='[;' read -p $'\e[6n' -d R -rs _ y x _
}

toPromptArea() {
	echo -ne "${ANSI_CUR_SAVE}\e[$((texture_height + 1));0H${ANSI_ERASE}"
}

toTexture() {
	echo -n "${ANSI_CUR_LOAD}"
}

promptQuit() {
	toPromptArea
	read -rsn1 -p "Save and quit? " input
	case $input in
		y) echo; exit 0;;
	esac
	toTexture
}

clear
echo -n "${ANSI_CUR_SAVE}${ANSI_CUR_HOME}"
displayTexture
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
		"q")  promptQuit;;
		"w")  displayTexture > "${texture_file%.*}.ctxt";;
		"[A") [ "$y" == 1 ] || echo -n "$ANSI_CUR_UP";;
		"[B") [ "$y" == "$texture_height" ] || echo -n "$ANSI_CUR_DOWN";;
		"[C") [ "$x" == "$texture_width" ] || echo -n "$ANSI_CUR_RIGHT";;
		"[D") [ "$x" == 1 ] || echo -n "$ANSI_CUR_LEFT";;
	esac
done
