#!/usr/bin/env bash
start="$PWD"

# get config from first line
read -r fg bg font size
: "${fg:=black}"
: "${bg:=white}"
: "${font:=Sans}"
: "${size:=1920x1080}"

# get font path
font="$(fc-list | grep -Fm1 "$(fc-match "$font" | cut -d: -f1)" | cut -d: -f1)"

present() {
	magick \
		-size "$size" \
		-fill "$fg" \
		-background "$bg" \
		-font "$font" \
		-gravity "northwest" \
		label:@- \
		"$1"
}

addBlock() {
	[ -z "$block" ] && return
	file="$ix.png"
	addFile "$file"
	((ix++))

	printf -- "%s" "$block" | present "$tmp/$file" &
	block=

}

addFile() {
	mpvFiles+=("$1")
	file -bi "$1" | grep -q "video\|audio" || pdfFiles+=("$1")
}

tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT

block=
ix=0
pdfFiles=()
mpvFiles=()
while IFS='' read -r line; do
	if [[ "$line" =~ ^@ ]]; then
		addBlock
		file="${line#@}"
		addFile "$file"

		ln -sr "$file" "$tmp"
	elif [ -z "$line" ]; then
		addBlock
	else
		block+="$line"$'\n'
	fi
done
wait

cd "$tmp" || exit 1
img2pdf -s "$size" "${pdfFiles[@]}" -o "$start/out.pdf"
echo "Shift+ENTER repeatable playlist-prev" | mpv \
	--loop=inf \
	--fullscreen=yes\
	--input-conf="/dev/stdin" \
	"${mpvFiles[@]}"
wait
