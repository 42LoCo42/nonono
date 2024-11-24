#!/usr/bin/env bash
now="$(date "+%s")"
max="$((14 * 24 * 3600))"
num="10"

curl \
	-sSLK "token" \
	"https://api.github.com/user/repos?per_page=$num&sort=pushed" \
| jq -r '.[] | "\(.name) \(.pushed_at)"' \
| while read -r name pushed_at; do
	time="$(date "+%s" --date="$pushed_at")"
	diff="$((now - time))"
	((diff > max)) && continue

	echo "$time	$name	$(date "+%d. %B %Y" --date="@$time")"
done \
| sort -rnk1 \
| sed 's|[^\t]*\t||' \
| column -ts '	'
