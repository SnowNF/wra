/*
 * Copyright (C) 2018 The Android Open Source Project
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

#include "DeviceMapper.h"
#include <sys/ioctl.h>
#include <sys/sysmacros.h>
#include <cstring>
#include <cstdint>
#include <unistd.h>

DeviceMapper::DeviceMapper() {
    fd = open("/dev/device-mapper", O_RDWR | O_CLOEXEC);
    if (fd < 0)
        wra_error("Failed to open device-mapper");
}

void DeviceMapper::InitIo(struct dm_ioctl *io, const std::string &name) {
    assert(io != nullptr);
    memset(io, 0, sizeof(*io));

    io->version[0] = DM_VERSION0;
    io->version[1] = DM_VERSION1;
    io->version[2] = DM_VERSION2;
    io->data_size = sizeof(*io);
    io->data_start = 0;
    if (!name.empty()) {
        snprintf(io->name, sizeof(io->name), "%s", name.c_str());
    }
}

bool DeviceMapper::GetDmDevicePathByName(const std::string &name, std::string *path) const {
    struct dm_ioctl io{};
    InitIo(&io, name);
    if (ioctl(fd, DM_DEV_STATUS, &io) < 0) {
        wra_error("DM_DEV_STATUS failed for %s", name.c_str());
        return false;
    }

    uint32_t dev_num = minor(io.dev);
    *path = "dm-" + std::to_string(dev_num);
//    *path = "/dev/block/dm-" + std::to_string(dev_num);
    return true;
}

bool DeviceMapper::GetDeviceNameAndUuid(dev_t dev, std::string *name, std::string *uuid) const {
    struct dm_ioctl io{};
    InitIo(&io, {});
    io.dev = dev;

    if (ioctl(fd, DM_DEV_STATUS, &io) < 0) {
        wra_error("Failed to find device dev: %d:%d", major(dev), minor(dev));
        return false;
    }

    if (name) {
        *name = io.name;
    }
    if (uuid) {
        *uuid = io.uuid;
    }
    return true;
}

DeviceMapper &DeviceMapper::Instance() {
    static DeviceMapper instance{};
    return instance;
}

static bool only_one_char(const uint8_t *buf, size_t len, uint8_t c) {
    for (int i = 0; i < len; i++) {
        if (buf[i] != c) {
            return false;
        }
    }
    return true;
}

bool DeviceMapper::partition_wiped(const char *source) {
    uint8_t buf[4096];
    int fd;

    if ((fd = open(source, O_RDONLY)) < 0) {
        wra_error("Unable to open %s", source);
        return false;
    }

    size_t ret = read(fd, buf, sizeof(buf));
    close(fd);

    if (ret != sizeof(buf)) {
        return false;
    }

    /* Check for all zeros */
    if (only_one_char(buf, sizeof(buf), 0)) {
        return true;
    }

    /* Check for all ones */
    if (only_one_char(buf, sizeof(buf), 0xff)) {
        return true;
    }

    return false;
}