#!/bin/bash

export SEATD_VTBOUND=0
/sbin/seatd&
/usr/lib/systemd/systemd-udevd &
UDEVD_PID=$!

SEATD_SOCK="/run/seatd.sock"
while [ ! -e "$SEATD_SOCK" ]; do
  echo "wait $SEATD_SOCK ..."
  sleep 1
done

chmod 777 "$SEATD_SOCK"

sleep 2

for device in /dev/input/event*; do
    SYSTEMD_IGNORE_CHROOT=1 udevadm trigger "$device"
    chmod 777 "$device"
done

kill $UDEVD_PID
echo kill systemd-udevd pid $UDEVD_PID

libinput list-devices | wc -l
echo "libinput list-devices" return $?

bash /wra/labwc.pixman.sh