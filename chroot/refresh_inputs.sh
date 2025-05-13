#!/bin/bash

/usr/lib/systemd/systemd-udevd &

sleep 1

for device in /dev/input/event*; do
    SYSTEMD_IGNORE_CHROOT=1 sudo -E  udevadm trigger "$device"
    sudo chmod 777 "$device"
done
libinput list-devices
