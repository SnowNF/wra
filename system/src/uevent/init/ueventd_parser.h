/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <string>
#include <vector>

#include "devices.h"
#include "firmware_handler.h"


namespace android::init {
    struct UeventdConfiguration {
        std::vector<Subsystem> subsystems = {
                Subsystem("graphics", Subsystem::DEVNAME_UEVENT_DEVPATH, "/dev/graphics"),
                Subsystem("drm", Subsystem::DEVNAME_UEVENT_DEVPATH, "/dev/dri"),
                Subsystem("input", Subsystem::DEVNAME_UEVENT_DEVPATH, "/dev/input"),
                Subsystem("sound", Subsystem::DEVNAME_UEVENT_DEVPATH, "/dev/snd"),
                Subsystem("dma_heap", Subsystem::DEVNAME_UEVENT_DEVPATH, "/dev/dma_heap"),
                Subsystem("vfio", Subsystem::DEVNAME_UEVENT_DEVPATH, "/dev/vfio"),
        };
        std::vector<std::string> firmware_directories = {"/etc/firmware/",
                                                         "/odm/firmware/",
                                                         "/vendor/firmware/",
                                                         "/firmware/image/",
                                                         "/vendor/firmware_mnt/image/"};
        size_t uevent_socket_rcvbuf_size = 16 * 1024 * 1024; //16MB
        std::vector<std::string> module_base_paths = {"/odm/lib/modules", "/vendor/lib/modules"};
    };
}

