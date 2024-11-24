#!/usr/bin/env -S bash -e
mkdir "$1"
mkdir -p "$1/var/lib/pacman/"{local,sync}
ln -s "/var/lib/pacman/local/ALPM_DB_VERSION" "$1/var/lib/pacman/local"
for i in "/var/lib/pacman/sync/"*; do
	ln -s "$i" "$1/var/lib/pacman/sync"
done
