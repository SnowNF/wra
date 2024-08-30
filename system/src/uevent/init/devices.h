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

#ifndef _INIT_DEVICES_H
#define _INIT_DEVICES_H

#include <sys/stat.h>
#include <sys/types.h>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "uevent.h"
#include "uevent_handler.h"
#include "main.h"
#include "utils.h"


namespace android::init {
    class Subsystem {
    public:
        enum DevnameSource {
            DEVNAME_UEVENT_DEVNAME, // uevent_devname
            DEVNAME_UEVENT_DEVPATH, // uevent_devpath
            DEVNAME_SYS_NAME,       // sys_name
        };

        Subsystem() = default;

        explicit Subsystem(std::string name) : name_(std::move(name)) {}

        Subsystem(std::string name, DevnameSource source, std::string dir_name)
                : name_(std::move(name)), devname_source_(source), dir_name_(std::move(dir_name)) {}

        // Returns the full path for a uevent of a device that is a member of this subsystem,
        // according to the rules parsed from ueventd.rc
        [[nodiscard]] std::string ParseDevPath(const Uevent &uevent) const {
            std::string devname;
            if (devname_source_ == DEVNAME_UEVENT_DEVNAME) {
                devname = uevent.device_name;
            } else if (devname_source_ == DEVNAME_UEVENT_DEVPATH) {
                devname = wra::Basename(uevent.path);
            } else if (devname_source_ == DEVNAME_SYS_NAME) {
                if (wra::ReadFileToString("/sys/" + uevent.path + "/name", &devname)) {
                    devname.pop_back();  // Remove terminating newline
                } else {
                    devname = uevent.device_name;
                }
            }
            return dir_name_ + "/" + devname;
        }

        bool operator==(const std::string &string_name) const { return name_ == string_name; }

    private:
        std::string name_;
        DevnameSource devname_source_ = DEVNAME_UEVENT_DEVNAME;
        std::string dir_name_ = "/dev";
    };

    class DeviceHandler : public UeventHandler {
    public:
        DeviceHandler();

        explicit DeviceHandler(std::vector<Subsystem> subsystems);

        ~DeviceHandler() override = default;

        void HandleUevent(const Uevent &uevent) override;

        // `androidboot.partition_map` allows associating a partition name for a raw block device
        // through a comma separated and semicolon deliminated list. For example,
        // `androidboot.partition_map=vdb,metadata;vdc,userdata` maps `vdb` to `metadata` and `vdc` to
        // `userdata`.
        static std::string GetPartitionNameForDevice(const std::string &device);

    private:

        bool FindPlatformDevice(std::string path, std::string *platform_device_path) const;

        [[nodiscard]] std::tuple<mode_t, uid_t, gid_t> GetDevicePermissions(
                const std::string &path, const std::vector<std::string> &links) const;

        void MakeDevice(const std::string &path, bool block, int major, int minor,
                        const std::vector<std::string> &links) const;

        [[nodiscard]] std::vector<std::string> GetBlockDeviceSymlinks(const Uevent &uevent) const;

        void HandleDevice(const std::string &action, const std::string &devpath, bool block, int major,
                          int minor, const std::vector<std::string> &links) const;

        void FixupSysPermissions(const std::string &upath, const std::string &subsystem) const;

        void HandleAshmemUevent(const Uevent &uevent);

        std::vector<Subsystem> subsystems_;
        std::set<std::string> boot_devices_ = {""};
        std::string sysfs_mount_point_ = "/sys";
    };

// Exposed for testing
    void SanitizePartitionName(std::string *string);

} // namespace android::init


#endif
