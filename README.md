# Wayland Replace Android

## Requirements

A rooted Qualcomm Android Device with DRM Support

### Chroot side
`labwc` (latest version),
`libinput` `sshd` `python3` ,installed patched turnip driver from [GitHub](https://github.com/SnowNF/mesa/tree/main-termux) and so on
### Termux side
`tsu` `screen` `sshpass` `ssh` and so on

## Usage
1. chroot side: put the chroot dir in /home
2. termux side: put the termux dir in /data/data/com.termux/files/home/
2. chroot side: `start sshd in chroot`
3. termux side: `sudo screen -R` # provent be killed by Android
4. termux side: `CHROOT_DIR_IN_CHROOT=/home/chroot CHROOT_PASSWD=<> CHROOT_USER=<> bash termux/run.sh`

## Reference
https://zhuanlan.zhihu.com/p/687261201

https://zhuanlan.zhihu.com/p/687939663