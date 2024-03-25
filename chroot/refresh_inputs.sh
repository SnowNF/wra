#!/bin/bash

for device in /dev/input/event*; do
    SYSTEMD_IGNORE_CHROOT=1 sudo -E udevadm test "$device"
    sudo chmod 777 "$device"
done
libinput list-devices