#!/bin/bash

######### global var #########

comp_pid="0"

######### functions #########

check_env_var(){
    var_name=$1
    default_value=$2

    if [ -z "${!var_name}" ]; then
        echo -n "$var_name is not set"
        if [ -n "$default_value" ]; then
            echo ", default to $default_value"
            export "$var_name"="$default_value"
        else
            echo
            exit 1
        fi
    else
        echo "$var_name is ${!var_name}"
    fi
}

set_adbd(){
    echo "setting adb port to $1"
    setprop service.adb.tcp.port "$1"
    echo "stopping adbd"
    stop adbd
    echo "starting adbd"
    start adbd
    echo "done"
}


set_comp_pid(){
    # shellcheck disable=SC2006
    result=(`lsof /dev/dri/card0 | grep ompos`)
	comp_pid=${result[1]}
    if [ -z "$comp_pid" ]; then
        echo "WARNING: unable to get composite pid"
    fi
}

exec_in_chroot(){
    sshpass -p"$CHROOT_PASSWD" ssh -l "$CHROOT_USER" 127.0.0.1 "$1"
}

######### main #########

# check & set
check_env_var "CHROOT_USER"
check_env_var "CHROOT_PASSWD"
check_env_var "CHROOT_DIR_IN_CHROOT"
check_env_var "SEATD_SH_PATH_IN_CHROOT" "$CHROOT_DIR_IN_CHROOT/seatd.sh"
check_env_var "LABWC_SH_PATH_IN_CHROOT" "$CHROOT_DIR_IN_CHROOT/labwc.vk.sh"
check_env_var "REFRESH_INPUTS_SH_PATH_IN_CHROOT" "$CHROOT_DIR_IN_CHROOT/refresh_inputs.sh"
check_env_var "ADB_PORT" "5555"

#set_adbd "$ADB_PORT"

set_comp_pid

exec_in_chroot "echo"
if [ $? != 0 ] 
then
    echo "unable to exec echo in chroot, is sshd running?"
    exit 1
fi

exec_in_chroot "bash $REFRESH_INPUTS_SH_PATH_IN_CHROOT"
if [ $? != 0 ] 
then
    echo "unable to refresh inputs, is libinput installed? or file exists?"
    exit 1
fi

exec_in_chroot "bash $SEATD_SH_PATH_IN_CHROOT"&

while true
do {
    sleep 2
    stop
    set_comp_pid
    kill "$comp_pid"
    exec_in_chroot "bash $LABWC_SH_PATH_IN_CHROOT"
} done
