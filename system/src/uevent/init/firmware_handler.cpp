/*
 * Copyright (C) 2017 The Android Open Source Project
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

#include "firmware_handler.h"
#include "logger.h"
#include "utils.h"

#include <fcntl.h>
#include <cstring>
#include <sys/sendfile.h>
#include <unistd.h>
#include <sstream>
#include <thread>
#include <main.h>
#include <sys/stat.h>


namespace android::init {
    static void LoadFirmware(const std::string &firmware, const std::string &root, int fw_fd,
                             size_t fw_size, int loading_fd, int data_fd) {
        // Start transfer.
        wra::WriteFully(loading_fd, "1", 1);

        // Copy the firmware.
        ssize_t rc = sendfile(data_fd, fw_fd, nullptr, fw_size);
        if (rc == -1)
            wra_error("firmware: sendfile failed { '%s', '%s' } %s fw_fd %d fw_size %zu loading_fd %d data_fd %d",
                      root.c_str(), firmware.c_str(), strerror(errno), fw_fd, fw_size, loading_fd, data_fd);

        // Tell the firmware whether to abort or commit.
        const char *response = (rc != -1) ? "0" : "-1";
        wra::WriteFully(loading_fd, response, strlen(response));
    }

    FirmwareHandler::FirmwareHandler(std::vector<std::string> firmware_directories)
            : firmware_directories_(std::move(firmware_directories)) {}

    std::string FirmwareHandler::GetFirmwarePath(const Uevent &uevent) {
        wra_info("firmware: loading { '%s', '%s' }", uevent.firmware.c_str(), uevent.path.c_str());
        return uevent.firmware;
    }

    void FirmwareHandler::ProcessFirmwareEvent(const std::string &path,
                                               const std::string &firmware) const {
        std::string root = "/sys" + path;
        std::string loading = root + "/loading";
        std::string data = root + "/data";

        int loading_fd(open(loading.c_str(), O_WRONLY | O_CLOEXEC));
        if (loading_fd == -1) {
            wra_error("couldn't open firmware loading fd for %s", firmware.c_str());
            return;
        }

        int data_fd(open(data.c_str(), O_WRONLY | O_CLOEXEC));
        if (data_fd == -1) {
            std::stringstream ss;
            ss << "couldn't open firmware data fd for " << firmware;
            wra_error("%s", ss.str().c_str());
            close(loading_fd);
            return;
        }

        std::vector<std::string> attempted_messages;
        auto TryLoadFirmware = [&](const std::string &firmware_directory) {
            std::string file = firmware_directory + firmware;
            int fw_fd(open(file.c_str(), O_RDONLY | O_CLOEXEC));
            if (fw_fd == -1) {
                attempted_messages.emplace_back("firmware: attempted " + file +
                                                ", open failed: " + strerror(errno));
                return false;
            }
            struct stat sb{};
            if (fstat(fw_fd, &sb) == -1) {
                attempted_messages.emplace_back("firmware: attempted " + file +
                                                ", fstat failed: " + strerror(errno));
                close(fw_fd);
                return false;
            }
            wra_info("found %s for %s", file.c_str(), path.c_str());
            LoadFirmware(firmware, root, fw_fd, sb.st_size, loading_fd, data_fd);
            wra_info("load %s successfully", file.c_str());
            close(fw_fd);
            return true;
        };

        int count = 0;

        while (true) {
            attempted_messages.clear();
            if (ForEachFirmwareDirectory(TryLoadFirmware)) {
                close(loading_fd);
                close(data_fd);
                return;
            }
            count++;
            if (count > 3)
                break;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        wra_error("firmware: could not find firmware for %s (%d times)", firmware.c_str(), count);
        for (const auto &message: attempted_messages) {
            wra_error("%s", message.c_str());
        }

        // Write "-1" as our response to the kernel's firmware request, since we have nothing for it.
        write(loading_fd, "-1", 2);
        close(loading_fd);
        close(data_fd);
    }

    bool FirmwareHandler::ForEachFirmwareDirectory(
            const std::function<bool(const std::string &)> &handler) const {
        for (const auto &firmware_directory: firmware_directories_) {
            if (std::invoke(handler, firmware_directory)) {
                return true;
            }
        }
        return false;
    }

    void FirmwareHandler::HandleUevent(const Uevent &uevent) {
        if (uevent.subsystem != "firmware" || uevent.action != "add") return;
        auto firmware = GetFirmwarePath(uevent);
        ProcessFirmwareEvent(uevent.path, firmware);
    }
}