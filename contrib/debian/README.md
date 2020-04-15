
Debian
====================
This directory contains files used to package ecodollard/ecodollar-qt
for Debian-based Linux systems. If you compile ecodollard/ecodollar-qt yourself, there are some useful files here.

## ecodollar: URI support ##


ecodollar-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install ecodollar-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your ecodollar-qt binary to `/usr/bin`
and the `../../share/pixmaps/ecodollar128.png` to `/usr/share/pixmaps`

ecodollar-qt.protocol (KDE)

