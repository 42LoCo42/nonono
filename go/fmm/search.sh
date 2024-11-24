#!/usr/bin/env bash
jq -r '
	.results[] |
	"\(.name)\(.title)"
' < mods.json \
| fzf -e -m -d '' --with-nth=2 --preview="jq -r '.results[] | select(.name == \"{1}\").summary' < mods.json" \
| sed 's|.*||'
