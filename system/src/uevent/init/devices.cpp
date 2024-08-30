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

#include "devices.h"

#include <cerrno>
#include <cstring>
#include <sys/sysmacros.h>
#include <unistd.h>

#include <filesystem>
#include <string>
#include <string_view>
#include <dirent.h>

#include <logger.h>
#include <main.h>
#include <DeviceMapper.h>

using namespace std::chrono_literals;


namespace android::init {

/* Given a path that may start with a PCI device, populate the supplied buffer
 * with the PCI domain/bus number and the peripheral ID and return 0.
 * If it doesn't start with a PCI device, or there is some error, return -1 */
    static bool FindPciDevicePrefix(const std::string &path, std::string *result) {
        result->clear();

        if (!wra::StartsWith(path, "/devices/pci")) return false;

        /* Beginning of the prefix is the initial "pci" after "/devices/" */
        std::string::size_type start = 9;

        /* End of the prefix is two path '/' later, capturing the domain/bus number
         * and the peripheral ID. Example: pci0000:00/0000:00:1f.2 */
        auto end = path.find('/', start);
        if (end == std::string::npos) return false;

        end = path.find('/', end + 1);
        if (end == std::string::npos) return false;

        auto length = end - start;
        if (length <= 4) {
            // The minimum string that will get to this check is 'pci/', which is malformed,
            // so return false
            return false;
        }

        *result = path.substr(start, length);
        return true;
    }

/* Given a path that may start with a virtual block device, populate
 * the supplied buffer with the virtual block device ID and return 0.
 * If it doesn't start with a virtual block device, or there is some
 * error, return -1 */
    static bool FindVbdDevicePrefix(const std::string &path, std::string *result) {
        result->clear();

        if (!wra::StartsWith(path, "/devices/vbd-")) return false;

        /* Beginning of the prefix is the initial "vbd-" after "/devices/" */
        std::string::size_type start = 13;

        /* End of the prefix is one path '/' later, capturing the
           virtual block device ID. Example: 768 */
        auto end = path.find('/', start);
        if (end == std::string::npos) return false;

        auto length = end - start;
        if (length == 0) return false;

        *result = path.substr(start, length);
        return true;
    }

// Given a path that may start with a virtual dm block device, populate
// the supplied buffer with the dm module's instantiated name.
// If it doesn't start with a virtual block device, or there is some
// error, return false.
    static bool FindDmDevice(const Uevent &uevent, std::string *name, std::string *uuid) {
        if (!wra::StartsWith(uevent.path, "/devices/virtual/block/dm-")) return false;
        if (uevent.action == "remove") return false;  // Avoid error spam from ioctl

        dev_t dev = makedev(uevent.major, uevent.minor);

        auto &dm = DeviceMapper::Instance();
        return dm.GetDeviceNameAndUuid(dev, name, uuid);
    }

    std::string DeviceHandler::GetPartitionNameForDevice(const std::string &query_device) {
        return {};
    }

// Given a path that may start with a platform device, find the parent platform device by finding a
// parent directory with a 'subsystem' symlink that points to the platform bus.
// If it doesn't start with a platform device, return false
    bool DeviceHandler::FindPlatformDevice(std::string path, std::string *platform_device_path) const {
        platform_device_path->clear();

        // Uevents don't contain the mount point, so we need to add it here.
        path.insert(0, sysfs_mount_point_);

        std::string directory = wra::Dirname(path);

        while (directory != "/" && directory != ".") {
            std::string subsystem_link_path;
            if (wra::Realpath(directory + "/subsystem", &subsystem_link_path) &&
                (subsystem_link_path == sysfs_mount_point_ + "/bus/platform" ||
                 subsystem_link_path == sysfs_mount_point_ + "/bus/amba")) {
                // We need to remove the mount point that we added above before returning.
                directory.erase(0, sysfs_mount_point_.size());
                *platform_device_path = directory;
                return true;
            }

            auto last_slash = path.rfind('/');
            if (last_slash == std::string::npos) return false;

            path.erase(last_slash);
            directory = wra::Dirname(path);
        }

        return false;
    }

    void DeviceHandler::FixupSysPermissions(const std::string &upath,
                                            const std::string &subsystem) const {
        // upaths omit the "/sys" that paths in this list
        // contain, so we prepend it...
        std::string path = "/sys" + upath;
        if (uEvent_Config.debug)
            wra_debug("FixupSysPermission %s", path.c_str());
    }

    std::tuple<mode_t, uid_t, gid_t> DeviceHandler::GetDevicePermissions(
            const std::string &path, const std::vector<std::string> &links) const {
        /* Default if nothing found. */
        return {0666, 0, 0};
    }

    void DeviceHandler::MakeDevice(const std::string &path, bool block, int major, int minor,
                                   const std::vector<std::string> &links) const {
        auto [mode, uid, gid] = GetDevicePermissions(path, links);
        mode |= (block ? S_IFBLK : S_IFCHR);

        gid_t new_group = -1;

        dev_t dev = makedev(major, minor);
        /* Temporarily change egid to avoid race condition setting the gid of the
         * device node. Unforunately changing the euid would prevent creation of
         * some device nodes, so the uid has to be set with chown() and is still
         * racy. Fixing the gid race at least fixed the issue with system_server
         * opening dynamic input devices under the AID_INPUT gid. */
        if (setegid(gid)) {
            std::stringstream ss;
            ss << "setegid(" << gid << ") for " << path << " device failed";
            wra_error("%s", ss.str().c_str());
            goto out;
        }
        /* If the node already exists update its SELinux label and the file mode to handle cases when
         * it was created with the wrong context and file mode during coldboot procedure. */
        if (mknod(path.c_str(), mode, dev) != 0) {
            if (errno != EEXIST || uEvent_Config.debug)
                wra_error("mknod failed for %s: %s", path.c_str(), ::strerror(errno));
        } else
            wra_info("added device %s", path.c_str());

        out:
        if (chown(path.c_str(), uid, new_group) < 0) {
            std::stringstream ss;
            ss << "Cannot chown " << path << " " << uid << " " << new_group;
            wra_error("%s", ss.str().c_str());
        }
    }

// replaces any unacceptable characters with '_', the
// length of the resulting string is equal to the input string
    void SanitizePartitionName(std::string *string) {
        const char *accept =
                "abcdefghijklmnopqrstuvwxyz"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "0123456789"
                "_-.";

        if (!string) return;

        std::string::size_type pos = 0;
        while ((pos = string->find_first_not_of(accept, pos)) != std::string::npos) {
            (*string)[pos] = '_';
        }
    }

    std::vector<std::string> DeviceHandler::GetBlockDeviceSymlinks(const Uevent &uevent) const {
        std::string device;
        std::string type;
        std::string partition;
        std::string uuid;

        if (FindPlatformDevice(uevent.path, &device)) {
            // Skip /devices/platform or /devices/ if present
            static constexpr std::string_view devices_platform_prefix = "/devices/platform/";
            static constexpr std::string_view devices_prefix = "/devices/";

            if (wra::StartsWith(device, devices_platform_prefix)) {
                device = device.substr(devices_platform_prefix.length());
            } else if (wra::StartsWith(device, devices_prefix)) {
                device = device.substr(devices_prefix.length());
            }

            type = "platform";
        } else if (FindPciDevicePrefix(uevent.path, &device)) {
            type = "pci";
        } else if (FindVbdDevicePrefix(uevent.path, &device)) {
            type = "vbd";
        } else if (FindDmDevice(uevent, &partition, &uuid)) {
            std::vector<std::string> symlinks = {"/dev/block/mapper/" + partition};
            if (!uuid.empty()) {
                symlinks.emplace_back("/dev/block/mapper/by-uuid/" + uuid);
            }
            return symlinks;
        } else {
            return {};
        }

        std::vector<std::string> links;

        if (uEvent_Config.debug)
            wra_debug("found new device [type %s] [device %s] [partition name %s] [path %s] [device name %s]",
                      type.c_str(), device.c_str(), uevent.partition_name.c_str(), uevent.path.c_str(),
                      uevent.device_name.c_str());

        auto link_path = "/dev/block/" + type + "/" + device;

        if (!uevent.partition_name.empty()) {
            std::string partition_name_sanitized(uevent.partition_name);
            SanitizePartitionName(&partition_name_sanitized);
            if (partition_name_sanitized != uevent.partition_name) {
                std::stringstream ss;
                ss << "Linking partition '" << uevent.partition_name << "' as '"
                   << partition_name_sanitized << "'";
                wra_debug("%s", ss.str().c_str());
            }
            links.emplace_back(link_path + "/by-name/" + partition_name_sanitized);
            // Adds symlink: /dev/block/by-name/<partition_name>.
            links.emplace_back("/dev/block/by-name/" + partition_name_sanitized);
        }

        std::string model;
        if (wra::ReadFileToString("/sys/class/block/" + uevent.device_name + "/queue/zoned", &model) &&
            !wra::StartsWith(model, "none")) {
            links.emplace_back("/dev/block/by-name/zoned_device");
            links.emplace_back("/dev/sys/block/by-name/zoned_device");
        }

        auto last_slash = uevent.path.rfind('/');
        links.emplace_back(link_path + "/" + uevent.path.substr(last_slash + 1));

        return links;
    }

    static void RemoveDeviceMapperLinks(const std::string &devpath) {
        std::vector<std::string> dirs = {
                "/dev/block/mapper",
                "/dev/block/mapper/by-uuid",
        };
        for (const auto &dir: dirs) {
            if (access(dir.c_str(), F_OK) != 0) continue;
            DIR *dh = ::opendir(dir.c_str());
            if (!dh) {
                std::stringstream ss;
                ss << "Failed to open directory " << dir;
                wra_error("%s", ss.str().c_str());
                continue;
            }

            struct dirent *dp;
            std::string link_path;
            while ((dp = readdir(dh)) != nullptr) {
                if (dp->d_type != DT_LNK) continue;

                auto path = dir + "/" + dp->d_name;
                if (wra::Readlink(path, &link_path) && link_path == devpath) {
                    unlink(path.c_str());
                }
            }
            ::closedir(dh);
        }
    }

    void DeviceHandler::HandleDevice(const std::string &action, const std::string &devpath, bool block,
                                     int major, int minor, const std::vector<std::string> &links) const {
        if (action == "add")
            MakeDevice(devpath, block, major, minor, links);


        // Handle device-mapper nodes.
        // On kernels <= 5.10, the "add" event is fired on DM_DEV_CREATE, but does not contain name
        // information until DM_TABLE_LOAD - thus, we wait for a "change" event.
        // On kernels >= 5.15, the "add" event is fired on DM_TABLE_LOAD, followed by a "change"
        // event.
        if (action == "add" || (action == "change" && wra::StartsWith(devpath, "/dev/block/dm-"))) {
            for (const auto &link: links) {
                std::string target;
                if (wra::StartsWith(link, "/dev/block/")) {
                    target = devpath;
                } else if (wra::StartsWith(link, "/dev/sys/block/")) {
                    target = "/sys/class/block/" + wra::Basename(devpath);
                } else {
                    std::stringstream ss;
                    ss << "Unrecognized link type: " << link;
                    wra_error("%s", ss.str().c_str());
                    continue;
                }

                if (!wra::mkdir_recursive(wra::Dirname(link), 0755)) {
                    std::stringstream ss;
                    ss << "Failed to create directory " << wra::Dirname(link);
                    wra_error("%s", ss.str().c_str());
                }

                if (symlink(target.c_str(), link.c_str())) {
                    if (errno != EEXIST) {
                        std::stringstream ss;
                        ss << "Failed to symlink " << devpath << " to " << link;
                        wra_error("%s", ss.str().c_str());
                    } else if (std::string link_path;
                            wra::Readlink(link, &link_path) && link_path != devpath) {
                        std::stringstream ss;
                        ss << "Failed to symlink " << devpath << " to " << link
                           << ", which already links to: " << link_path;
                        wra_error("%s", ss.str().c_str());
                    }
                } else
                    wra_info("link device %s -> %s", target.c_str(), link.c_str());
            }
        }

        if (action == "remove") {
            if (wra::StartsWith(devpath, "/dev/block/dm-")) {
                RemoveDeviceMapperLinks(devpath);
            }
            for (const auto &link: links) {
                std::string link_path;
                if (wra::Readlink(link, &link_path) && link_path == devpath) {
                    unlink(link.c_str());
                }
            }
            unlink(devpath.c_str());
        }
    }

    void DeviceHandler::HandleAshmemUevent(const Uevent &uevent) {
        if (uevent.device_name == "ashmem") {
            static const std::string boot_id_path = "/proc/sys/kernel/random/boot_id";
            std::string boot_id;
            if (!wra::ReadFileToString(boot_id_path, &boot_id)) {
                wra_error("Cannot duplicate ashmem device node. Failed to read %s", boot_id_path.c_str());
                return;
            }
            boot_id = wra::Trim(boot_id);

            Uevent dup_ashmem_uevent = uevent;
            dup_ashmem_uevent.device_name += boot_id;
            dup_ashmem_uevent.path += boot_id;
            HandleUevent(dup_ashmem_uevent);
        }
    }

    void DeviceHandler::HandleUevent(const Uevent &uevent) {
        if (uevent.action == "add" || uevent.action == "change" ||
            uevent.action == "bind" || uevent.action == "online") {
            FixupSysPermissions(uevent.path, uevent.subsystem);
        }

        // if it's not a /dev device, nothing to do
        if (uevent.major < 0 || uevent.minor < 0) return;

        std::string devpath;
        std::vector<std::string> links;
        bool block = false;

        if (uevent.subsystem == "block") {
            block = true;
            devpath = "/dev/block/" + wra::Basename(uevent.path);

            if (wra::StartsWith(uevent.path, "/devices")) {
                links = GetBlockDeviceSymlinks(uevent);
            }
        } else if (const auto subsystem =
                    std::find(subsystems_.cbegin(), subsystems_.cend(), uevent.subsystem);
                subsystem != subsystems_.cend()) {
            devpath = subsystem->ParseDevPath(uevent);
        } else if (uevent.subsystem == "usb") {
            if (!uevent.device_name.empty()) {
                devpath = "/dev/" + uevent.device_name;
            } else {
                // This imitates the file system that would be created
                // if we were using devfs instead.
                // Minors are broken up into groups of 128, starting at "001"
                int bus_id = uevent.minor / 128 + 1;
                int device_id = uevent.minor % 128 + 1;
                size_t len = snprintf(nullptr, 0, "/dev/bus/usb/%03d/%03d", bus_id, device_id) + 1;
                char *str = (char *) alloca(len);
                snprintf(str, len, "/dev/bus/usb/%03d/%03d", bus_id, device_id);
                devpath = std::string(str);
            }
        } else if (wra::StartsWith(uevent.subsystem, "usb")) {
            // ignore other USB events
            return;
        } else if (uevent.subsystem == "misc" && wra::StartsWith(uevent.device_name, "dm-user/")) {
            devpath = "/dev/dm-user/" + uevent.device_name.substr(8);
        } else if (uevent.subsystem == "misc" && uevent.device_name == "vfio/vfio") {
            devpath = "/dev/" + uevent.device_name;
        } else {
            devpath = "/dev/" + wra::Basename(uevent.path);
        }

        wra::mkdir_recursive(wra::Dirname(devpath), 0755);

        HandleDevice(uevent.action, devpath, block, uevent.major, uevent.minor, links);

        // Duplicate /dev/ashmem device and name it /dev/ashmem<boot_id>.
        HandleAshmemUevent(uevent);
    }

    DeviceHandler::DeviceHandler(std::vector<Subsystem> subsystems)
            : subsystems_(std::move(subsystems)) {}

    DeviceHandler::DeviceHandler()
            : DeviceHandler(
            std::vector<Subsystem>{}) {}

}

