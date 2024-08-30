#!/bin/bash

echo "Running $0"

bash /wra/display.sh &
bash /wra/cancel_trigger.sh &

export SYSTEMD_IGNORE_CHROOT=1

SYSTEMD_FILE=/wra/.systemd
while true; do
  if [ -e "$SYSTEMD_FILE" ]; then
    echo "Init: $SYSTEMD_FILE exists, prepare to run systemd"
    break
  fi
  sleep 5
done

rm $SYSTEMD_FILE

echo '# stub for immediately telling the kernel that userspace firmware loading
# failed; necessary to avoid long timeouts with CONFIG_FW_LOADER_USER_HELPER=y
# diabled by wra, do NOT edit
#SUBSYSTEM=="firmware", ACTION=="add", ATTR{loading}="-1"' >/usr/lib/udev/rules.d/50-firmware.rules

echo "Exec /sbin/init"
exec /sbin/init --log-level=debug --log-target=kmsg --default-standard-output=kmsg --default-standard-error=kmsg
