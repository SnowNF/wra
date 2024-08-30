#!/bin/bash

PID=$(cat /wra/.trigger)

export XDG_RUNTIME_DIR=/tmp

while true; do
  yad --title="Panic Trigger" --text="     Panic Trigger is running in pid $PID.     \n     Press OK to stop it.     " --button=OK

  if [ $? -eq 0 ]; then

    rm /wra/.trigger
    touch /wra/.trigger
  
    exit 0
  fi

  # wait for wayland compositor started
  sleep 1
done
