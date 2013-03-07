#!/bin/sh

if [ ! $1 ]; then
	dd if=/dev/zero of=dummy0 bs=1b count=182
	dd if=/dev/zero of=dummy1 bs=1b count=125
	dd if=/dev/zero of=dummy2 bs=1b count=162
	exit
elif [ $1 = "-rm" ]; then
	rm dummy0 dummy1 dummy2
fi

