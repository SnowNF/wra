#!/bin/bash

SYSTEMD_FILE=/wra/.systemd

echo touch $SYSTEMD_FILE

sudo touch $SYSTEMD_FILE

echo return $?

sudo dmesg

sleep 100