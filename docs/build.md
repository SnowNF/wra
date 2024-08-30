# Dependencies

To build android-tools, we need `libgtest-dev` `cmake` etc

To create an debian rootfs, we need `debootstrap`

To run aarch64 programs in rootfs, we need `qemu-user-static`

```shell
sudo apt install libgtest-dev debootstrap cmake qemu-user-static
```

# Preparaton

`gsi.img` in `build` directory, in order to extract `system/bin/linker64` and `system/lib64`

# Build
```shell
sudo ./scripts/build_all.sh
```

If you want build step by step, you can refer to `scripts/build_all.sh`