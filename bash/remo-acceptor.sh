#!/usr/bin/env nix-shell
port=18213
unix="/tmp/remo"

if [ "$1" == "child" ]; then
	notify-send -a remo -u critical "Prohn-Alarm!"

	rm -f "$unix"
	trap 'rm -f "$unix"' EXIT
	socat UNIX-LISTEN:"$unix" GOPEN:/dev/null

	cat << EOF
HTTP/1.1 200 OK

accepted
EOF
else
	echo "Listening..."
	while true; do
		socat TCP4-LISTEN:"$port",reuseaddr EXEC:"$0 child" || exit 1
	done
fi
