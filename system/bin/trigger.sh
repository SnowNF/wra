#!/bin/busybox sh
echo "Panic Trigger: pid $$"

PID_FILE="/data/root/wra/.trigger"

echo "Panic Trigger: writing pid $$ to $PID_FILE"
echo $$ > $PID_FILE

chmod 666 $PID_FILE

echo "Panic Trigger: test 60s"

MAX_WAIT=60
CURRENT_WAIT=0

while [ $CURRENT_WAIT -lt $MAX_WAIT ]; do
  if [ -e "$PID_FILE" ]; then
    if [ ! -s "$PID_FILE" ]; then
      echo "Panic Trigger: $PID_FILE exists && is empty..."
      exit 0
    fi
  fi
  sleep 1
  CURRENT_WAIT=$((CURRENT_WAIT + 1))
done

echo "Panic Trigger: sync"
/bin/busybox sync
echo "Panic Trigger: umount"
/bin/busybox umount -l -f /data/
echo "Panic Trigger: echo 1 > /proc/sys/kernel/sysrq"
echo 1 > /proc/sys/kernel/sysrq
echo "Panic Trigger: echo c > /proc/sysrq-trigger"
echo c > /proc/sysrq-trigger
