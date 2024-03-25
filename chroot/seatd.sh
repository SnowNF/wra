#!/bin/bash

set_permission(){
    sleep 1
    sudo chmod 777 /run/seatd.sock
}

set_permission&

export SEATD_VTBOUND=0
sudo -E seatd&