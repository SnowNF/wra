cmake_minimum_required(VERSION 3.25)
project(wra)

set(CMAKE_CXX_STANDARD 20)

link_libraries("-static")

add_executable(ueventd
        uevent/main.h
        uevent/main.cpp

        uevent/init/uevent.h
        uevent/init/uevent_handler.h
        uevent/init/uevent_listener.cpp
        uevent/init/uevent_listener.h
        uevent/init/ueventd.cpp
        uevent/init/ueventd.h

        uevent/init/devices.h
        uevent/init/devices.cpp
        uevent/init/modalias_handler.h
        uevent/init/modalias_handler.cpp
        uevent/init/firmware_handler.h
        uevent/init/firmware_handler.cpp
        uevent/init/ueventd_parser.h

        uevent/libcutils/uevent.h
        uevent/libcutils/uevent.cpp
        uevent/libmodprobe/libmodprobe.cpp
        uevent/libmodprobe/libmodprobe_ext.cpp
        uevent/libmodprobe/modprobe.h

        utils/logger.h
        utils/utils.cpp
        utils/utils.h
        utils/DeviceMapper.cpp
        utils/DeviceMapper.h
)

target_include_directories(ueventd PRIVATE
        utils
        uevent
)

add_executable(init
        init/init.c
)

add_executable(wra-utils
        wra-utils/main.cpp

        utils/logger.h
        utils/DeviceMapper.cpp
        utils/DeviceMapper.h
        utils/utils.cpp
        utils/utils.h

        uevent/libmodprobe/libmodprobe.cpp
        uevent/libmodprobe/libmodprobe_ext.cpp
        uevent/libmodprobe/modprobe.h
)

target_include_directories(wra-utils PRIVATE
        utils
        uevent
)

add_executable(drm-test
        drm-test/drm-test.cpp
        drm-test/drm-api.c
        drm-test/drm-api.h

        drm-test/libdrm/xf86drm.c
        drm-test/libdrm/xf86drmMode.c
)

target_include_directories(drm-test PRIVATE
        drm-test/libdrm/)