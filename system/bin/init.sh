#!/bin/busybox sh

wra_exec() {
    echo "INIT: $*"
    $* 2>&1
    local return_code=$?
    echo "INIT: $* finished with return code: $return_code"
    return $return_code
}

busybox_exec() {
    local cmd="/bin/busybox $@"
    wra_exec "$cmd"
}

capture_stderr() {
    output=$( $* 2>&1 )
    echo "$output"
}

echo "INIT: $0"

wra_exec /bin/wra-utils GetDmDevicePathByName userdata_gsi

sleep 1

echo "INIT: echo 8 4 1 7 > /proc/sys/kernel/printk"
echo "8 4 1 7" > /proc/sys/kernel/printk

#
#   Create /dev/ Nodes
#

wra_exec /bin/ueventd --no-loop

wra_exec /bin/ueventd --no-cold-boot&

wra_exec /bin/drm-test

#
#   Prepare /data Mount Point
#

wra_exec /bin/wra-utils GetDmDevicePathByName userdata_gsi

/bin/wra-utils FormatPartitionIfWiped "/dev/block/$(capture_stderr /bin/wra-utils GetDmDevicePathByName userdata_gsi s)"

busybox_exec mount "/dev/block/$(capture_stderr /bin/wra-utils GetDmDevicePathByName userdata_gsi s)" /data

wra_exec cat /data/testFile

echo "test" >> /data/testFile

wra_exec cat /data/testFile

#
#   Mount necessary Points
#

busybox_exec mkdir -p /dev/shm
busybox_exec mount -t tmpfs tmpfs /dev/shm

busybox_exec mkdir -p /mnt/vendor/persist

# TODO DO NOT HARD CODE
attempt=0
while [ $attempt -le 3 ]; do
    echo "attempt mount $attempt times"
    wra_exec /bin/busybox mount /dev/block/by-name/persist /mnt/vendor/persist -o ro
    wra_exec /bin/busybox mount /dev/block/by-name/modem_a /vendor/firmware_mnt -o ro
    if [ $? -eq 0 ]; then
        break
    fi
    attempt=$((attempt + 1))
    sleep 1
done

#
#   Decompress Rootfs
#

if [ ! -d "/data/root" ]; then
    echo "/data/root does not exist, decompressing..."
    busybox_exec tar xvfz /system/rootfs.tgz -C /data
    busybox_exec mv /data/rootfs /data/root
    busybox_exec sync
else
    echo "/data/root exists"
    busybox_exec ls /data/root
    busybox_exec ls /data/root/usr
    busybox_exec ls /data/root/wra
    busybox_exec ls /data/root/run
fi

busybox_exec sh /system/ext/qualcomm_cnss.sh stage1

wra_exec /bin/wra-utils LoadListedModules --base-path /odm/lib/modules --base-path /vendor/lib/modules

busybox_exec sh /system/ext/qualcomm_cnss.sh stage2

busybox_exec free -h

busybox_exec sh /system/bin/trigger.sh&

#
#   Prepare to Exec Init
#

busybox_exec env

echo "Propare to exec init"

# Prepare the /dev directory
[ ! -h /dev/fd ] && busybox_exec ln -s /proc/self/fd /dev/fd
[ ! -h /dev/stdin ] && busybox_exec ln -s /proc/self/fd/0 /dev/stdin
[ ! -h /dev/stdout ] && busybox_exec ln -s /proc/self/fd/1 /dev/stdout
[ ! -h /dev/stderr ] && busybox_exec ln -s /proc/self/fd/2 /dev/stderr

unset SHLVL
unset FIRST_STAGE_STARTED_AT
unset MOUNT_SYSTEM_OK
unset INIT_AVB_VERSION
unset LD_PRELOAD

# IMPORTANT
busybox_exec mount -o bind -o rshared /data/root /data/root

busybox_exec umount /tmp
busybox_exec mount -t tmpfs tmpfs /tmp
busybox_exec chmod -R 777 /tmp

busybox_exec mount -o rbind /dev /data/root/dev
busybox_exec mount -o rbind /proc /data/root/proc
busybox_exec mount -o rbind /sys /data/root/sys
busybox_exec mount -o rbind /tmp /data/root/tmp

export PATH=/sbin:/usr/sbin:/bin:/usr/bin

echo "exec /bin/busybox chroot /data/root /bin/bash /wra/init.sh"

# shellcheck disable=SC2093
exec /bin/busybox chroot /data/root /bin/bash /wra/init.sh

busybox_exec reboot