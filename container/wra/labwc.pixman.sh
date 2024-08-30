#!/bin/bash

cpu_render_info(){
while true; do
  XDG_RUNTIME_DIR=/tmp yad --title="CPU Rendering Notice" --text="     You are now in CPU rendering.     \n     Perfrormance may not good.     " --button=OK

  if [ $? -eq 0 ]; then
    break
  fi

  # wait for wayland compositor started
  sleep 1
done

}

# Loop to avoid labwc exit
# TODO: add refresh inputs support?
# TODO: add zink and its fallback support
while true; do
  chmod -R 777 /dev/dri/
  /bin/busybox su -l wra -c "XDG_RUNTIME_DIR=/tmp WLR_RENDERER=vulkan TU_FORCE_KGSL_DRM=1 labwc"

  echo labwc vulkan exited with $?
  cpu_render_info&

  chmod -R 777 /dev/dri/
  /bin/busybox su -l wra -c "XDG_RUNTIME_DIR=/tmp WLR_RENDERER=pixman labwc"
  LABWC_RET=$?
  echo labwc pixman exited with $LABWC_RET
  if [ $LABWC_RET -ne 0 ]; then
    break
  fi
done