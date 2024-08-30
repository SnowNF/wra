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

#include <sys/stat.h>
#include <sys/syscall.h>

#include <main.h>
#include <logger.h>

#include <libmodprobe/modprobe.h>
#include <fcntl.h>
#include <unistd.h>
#include "utils.h"

std::string Modprobe::GetKernelCmdline() {
    std::string cmdline;
    if (!wra::ReadFileToString("/proc/cmdline", &cmdline)) {
        return "";
    }
    return cmdline;
}

bool Modprobe::Insmod(const std::string &path_name, const std::string &parameters) {
    int fd(TEMP_FAILURE_RETRY(open(path_name.c_str(), O_RDONLY | O_NOFOLLOW | O_CLOEXEC)));
    if (fd == -1) {
        wra_error("Could not open module '%s'", path_name.c_str());
        return false;
    }

    auto canonical_name = MakeCanonical(path_name);
    std::string options;
    auto options_iter = module_options_.find(canonical_name);
    if (options_iter != module_options_.end()) {
        options = options_iter->second;
    }
    if (!parameters.empty()) {
        options = options + " " + parameters;
    }

    wra_info("Loading module %s with args '%s'", path_name.c_str(), options.c_str());
    long ret = syscall(__NR_finit_module, fd, options.c_str(), 0);
    if (ret != 0) {
        if (errno == EEXIST) {
            // Module already loaded
            std::lock_guard guard(module_loaded_lock_);
            module_loaded_paths_.emplace(path_name);
            module_loaded_.emplace(canonical_name);
            close(fd);
            wra_info("Kernel module %s already loaded", path_name.c_str());
            return true;
        }
        wra_error("Failed to insmod '%s' with args '%s'", path_name.c_str(), options.c_str());
        close(fd);
        return false;
    }

    wra_info("Loading kernel module %s successfully", path_name.c_str());
    std::lock_guard guard(module_loaded_lock_);
    module_loaded_paths_.emplace(path_name);
    module_loaded_.emplace(canonical_name);
    module_count_++;
    close(fd);
    return true;
}

bool Modprobe::Rmmod(const std::string &module_name) {
    auto canonical_name = MakeCanonical(module_name);
    long ret = syscall(__NR_delete_module, canonical_name.c_str(), O_NONBLOCK);
    if (ret != 0) {
        wra_error("Failed to remove module '%s'", module_name.c_str());
        return false;
    }
    std::lock_guard guard(module_loaded_lock_);
    module_loaded_.erase(canonical_name);
    return true;
}

bool Modprobe::ModuleExists(const std::string &module_name) {
    struct stat fileStat{};
    if (blocklist_enabled && module_blocklist_.count(module_name)) {
        wra_info("module %s is blocklisted", module_name.c_str());
        return false;
    }
    auto deps = GetDependencies(module_name);
    if (deps.empty()) {
        // missing deps can happen in the case of an alias
        return false;
    }
    if (stat(deps.front().c_str(), &fileStat)) {
        wra_info("module %s can't be loaded; can't access %s", module_name.c_str(), deps.front().c_str());
        return false;
    }
    if (!S_ISREG(fileStat.st_mode)) {
        wra_info("module %s is not a regular file", module_name.c_str());
        return false;
    }
    return true;
}
