# Wayland Replace Android

## 1. Project Description

### 1.1 What Is This?

WRA is a very experimental little project to make bootable GNU/Linux GSIs
for Android devices (mainly Qualcomm).

The goal is to enable GNU/Linux booting and basic functionality on these devices
without the need to recompile the Android kernel.

### 1.2 Why?

Simply for fun and experimentation

## 2. Build

Refer to the [build.md](docs/build.md) file for detailed build instructions.

## 3.Run

> Do not use it on a device with important data; use at your own risk.

After the build is complete, just copy `build/system.img` to your device
and then use [DSU-Sideloader](https://github.com/VegaBobo/DSU-Sideloader) to flash
and run it