#!/usr/bin/env bash
(($# < 1)) && echo "Usage: $0 <image>" && exit 1

read -r term_height term_width < <(stty size)
((term_height *= 2)) # 2 pixels per row

read -r img_width img_height < <(identify -format "%w %h\n" "$1")

if ((img_width / term_width > img_height / term_height)); then
	((img_height = term_width * img_height / img_width))
	((img_width = term_width))
else
	((img_width = term_height * img_width / img_height))
	((img_height = term_height))
fi

convert "$1" -adaptive-resize "${img_width}x${img_height}" "out"
clear
2ff < "out" | ./main "$(identify -format "%w" out)"
