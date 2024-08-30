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

#ifndef WRA_DEVICEMAPPER_H
#define WRA_DEVICEMAPPER_H

#include <linux/dm-ioctl.h>
#include "../utils/logger.h"
#include <fcntl.h>
#include <string>


// The minimum expected device mapper major.minor version
#define DM_VERSION0 (4)
#define DM_VERSION1 (0)
#define DM_VERSION2 (0)

class DeviceMapper {

    int fd = -1;
public:
    DeviceMapper();

    static DeviceMapper &Instance();

    static void InitIo(struct dm_ioctl *io, const std::string &name);

    bool GetDmDevicePathByName(const std::string &name, std::string *path) const;

    bool GetDeviceNameAndUuid(dev_t dev, std::string *name, std::string *uuid) const;

    static bool partition_wiped(const char *source);
};

#endif //WRA_DEVICEMAPPER_H
