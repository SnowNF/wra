#!/bin/busybox sh

wra_exec() {
    echo "QCNSS: $*"
    $* 2>&1
    local return_code=$?
    echo "QCNSS: $* finished with return code: $return_code"
    return $return_code
}

unset LD_PRELOAD

VENDOR_DIR=/data/vendor/wifi/

stage2(){
    if [ ! -d "$VENDOR_DIR" ]; then
        echo "$VENDOR_DIR does not exist, creating..."
        mkdir -p $VENDOR_DIR
        mkdir -p "$VENDOR_DIR/sockets"
        wra_exec /bin/busybox dd if=/dev/zero of=/data/vendor/wifi/iotap_ps.bin bs=120 count=1
        wra_exec /system/bin/linker64 --list /vendor/bin/cnss-daemon
    else
        echo "$VENDOR_DIR exists"
    fi

    # wra_exec /system/bin/wlan_control ON
    # wra_exec /system/bin/wlan_control OFF
    # wra_exec /system/bin/wlan_control ON
    wra_exec /vendor/bin/cnss-daemon -n -dddd&
}

# form /vendor/etc/init/hw/init.target.rc
stage1(){
    echo "QCNSS: echo 8 4 1 7 > /proc/sys/kernel/printk"
    echo "8 4 1 7" > /proc/sys/kernel/printk

    wra_exec /vendor/bin/qrtr-lookup
    wra_exec /vendor/bin/qrtr-ns -f&
    # wra_exec /bin/busybox chroot /data/root /usr/bin/qrtr-ns -f&

    echo "QCNSS: echo 1 > /dev/ipa"

    wra_exec /vendor/bin/qrtr-lookup

    # echo 1 > /proc/sys/kernel/firmware_config/force_sysfs_fallback
    echo 1 > /dev/ipa
    echo 1 > /sys/kernel/boot_adsp/boot
    # echo 1 > /sys/kernel/boot_cdsp/boot
    
    sleep 5

    # Enable WLAN cold boot calibration
    # Enable WLAN SSR recovery
    # echo "QCNSS: echo 1 > /sys/kernel/cnss/recovery"
    # echo 1 > /sys/kernel/cnss/recovery
    
    # wra_exec /vendor/bin/ipacm
    wra_exec /vendor/bin/qrtr-lookup

    # echo 1 > /sys/kernel/boot_cdsp/boot
    # echo 1 > /sys/devices/virtual/npu/msm_npu/boot
    # echo 1 > /sys/devices/virtual/cvp/cvp/boot
    
    stage2
    echo "sleep 10"
    sleep 10
    echo "QCNSS: echo 1 > /sys/kernel/cnss/fs_ready"
    echo 1 > /sys/kernel/cnss/fs_ready
    wra_exec /vendor/bin/qrtr-lookup
    echo "sleep 10?"
}

case "$1" in
    stage1)
        # stage2&
        # sleep 1
        stage1
        ;;
    stage2)
        # wra_exec /system/bin/wlan_control ON
        # wra_exec /system/bin/wlan_control OFF
        # wra_exec /system/bin/wlan_control ON
        ;;
    *)
        echo "Usage: $0 {stage1|stage2}"
        exit 1
        ;;
esac