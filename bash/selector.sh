#!/usr/bin/env bash
mapfile -t elements < "$1"
declare -A choices

while true; do
	echo -n '[2J[H'
	for i in $(seq 0 "$((${#elements[@]} - 1))"); do
		echo "${choices[$i]}$i: ${elements[i]}[m"
	done
	echo
	read -rp "Select elements with numbers. d <number> to deselect. q to quit."$'\n' sel arg

	if [ "$sel" == "q" ]; then
		break
	elif [ "$sel" == "d" ]; then
		unset "choices[$arg]"
	else
		choices[$sel]="[32m"
	fi
done

for i in "${!choices[@]}"; do echo "$i"; done
