#!/usr/bin/env bash
disk="/dev/vda"
host="archbox"
user="user"
timezone="Europe/Berlin"
locale="en_US.UTF-8"
keymap="de-latin1"
sudo="NOPASSWD:"

pacman-conf() {
	sed -Ei '
		s|^#Color|Color\nILoveCandy|;
		s|^#ParallelDownloads.*|ParallelDownloads = 10|;
	' "/etc/pacman.conf"
}

if (($# == 0)); then
	timedatectl set-ntp true

	sgdisk "$disk" -Z \
		-n 0:0:+8M -t 0:ef00 \
		-N 0       -t 0:8304
	mkfs.vfat "${disk}1"
	yes | mkfs.ext4 "${disk}2"

	mount "${disk}2" "/mnt"
	mkdir -p "/mnt/boot/efi"
	mount "${disk}1" "/mnt/boot/efi"

	pacman-conf
	pacstrap "/mnt" \
		base base-devel linux \
		refind booster \
		dhcpcd \
		neovim \
		man-db man-pages \
		git htop bash-completion

	genfstab -U "/mnt" >> "/mnt/etc/fstab"

	chmod +x "${BASH_SOURCE[0]}"
	cp "${BASH_SOURCE[0]}" "/mnt"
	arch-chroot "/mnt" "/${BASH_SOURCE[0]}" a
	rm "/mnt/${BASH_SOURCE[0]}"
else
	refind-install
	sed -Ei '
		s|^timeout 20$|timeout -1|;
		s|^#(textonly)|\1|;
	' "/boot/efi/EFI/refind/refind.conf"
	uuid="$(
		blkid "${disk}2" -o export \
		| sed -n 's|^UUID=||p'
	)"
	cat << EOF > "/boot/refind_linux.conf"
"Default" "ro root=UUID=$uuid initrd=boot\booster-linux.img loglevel=4"
EOF

	ln -sf "/usr/share/zoneinfo/$timezone" "/etc/localtime"
	hwclock --systohc

	sed -Ei "
		s|^#(en_US.UTF-8)|\1|;
		s|^#($locale)|\1|;
	" "/etc/locale.gen"
	cat << EOF > "/etc/locale.conf"
LANG=$locale
LC_COLLATE=C
EOF
	echo "KEYMAP=$keymap" > "/etc/vconsole.conf"
	locale-gen

	echo "$host" > "/etc/hostname"

	ln -sf "/usr/bin/nvim" "/usr/bin/vim"

	systemctl enable dhcpcd

	passwd -dl "root"
	useradd -mUG audio,video,input,wheel "$user"
	yes " " | passwd "$user"
	echo "%wheel ALL=(ALL:ALL) $sudo ALL" > "/etc/sudoers.d/wheel"

	pacman-conf
	cat << EOF | sudo -iu "$user"
git clone https://aur.archlinux.org/yay-bin
cd yay-bin
yes | makepkg -i
cd ..
rm -rf yay-bin
EOF
fi
