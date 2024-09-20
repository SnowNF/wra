#!/bin/bash

cd "$(dirname "$0")" || exit

source _base_env.sh

mkdir -p $BUILD_ROOT

cd $BUILD_ROOT

main() {
    set -x
    chroot rootfs /bin/su -l root -c /bin/bash -c "chmod -R 777 /tmp"
    chroot rootfs /bin/su -l root -c /bin/bash -c "apt-get update"
    chroot rootfs /bin/su -l root -c /bin/bash -c "apt-get install -y sudo"
    chroot rootfs /bin/su -l root -c /bin/bash -c "apt-get install -y bash-completion"
    chroot rootfs /bin/su -l root -c /bin/bash -c "useradd wra -m" | true
#    chroot rootfs /bin/su -l root -c /bin/bash -c "echo wra:wra | chpasswd"
    chroot rootfs /bin/su -l root -c /bin/bash -c "chsh -s /bin/bash wra && echo 'wra ALL=(ALL) NOPASSWD:ALL' > /etc/sudoers.d/wra"
    chroot rootfs /bin/su -l root -c /bin/bash -c "apt-get install -y libqt6widgets6 libqt6xml6" # control-panel
    chroot rootfs /bin/su -l root -c /bin/bash -c "apt-get install -y qterminal"
    chroot rootfs /bin/su -l root -c /bin/bash -c "apt-get install -y labwc pcmanfm-qt dbus-x11 --no-install-suggests --no-install-recommends"
    chroot rootfs /bin/su -l root -c /bin/bash -c "apt-get install -y yad --no-install-suggests --no-install-recommends"
    chroot rootfs /bin/su -l root -c /bin/bash -c "apt-get install -y cmst"
    chroot rootfs /bin/su -l root -c /bin/bash -c "apt-get install -y wvkbd"
    chroot rootfs /bin/su -l root -c /bin/bash -c "apt-get install -y seatd libinput-tools"
    chroot rootfs /bin/su -l root -c /bin/bash -c "apt-get install -y busybox-static"
    chroot rootfs /bin/su -l wra -c /bin/bash -c "mkdir -p /home/wra/.config/labwc"
    chroot rootfs /bin/su -l wra -c /bin/bash -c "cp -f /usr/share/doc/labwc/rc.xml /home/wra/.config/labwc/"
    cp -f $PROJECT_ROOT/container/menu.xml rootfs/home/wra/.config/labwc/
    chroot rootfs /bin/su -l root -c /bin/bash -c "chmod -R 777 /home/wra/.config/labwc"

    rm -r rootfs/wra | true
    cp -r $PROJECT_ROOT/container/wra rootfs/
    chmod -R 777 $PROJECT_ROOT/container/wra rootfs/wra
    chroot rootfs /bin/su -l root -c /bin/bash -c "usermod -a -G cdrom,floppy,sudo,audio,dip,video,plugdev,users,render,netdev,bluetooth,input wra"
    set +x
}

mount --bind /proc rootfs/proc
mount --bind /dev rootfs/dev # for apt-get update
_notice_ "Configuring rootfs"
main || true
_notice_ "Finished"
umount rootfs/dev
umount rootfs/proc
