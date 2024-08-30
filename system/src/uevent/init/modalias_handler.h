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

#pragma once

#include <string>
#include <vector>

#include <libmodprobe/modprobe.h>

#include "uevent.h"
#include "uevent_handler.h"


namespace android::init {

    class ModaliasHandler : public UeventHandler {
    public:
        explicit ModaliasHandler(const std::vector<std::string> &);

        ~ModaliasHandler() override = default;

        void HandleUevent(const Uevent &uevent) override;

    private:
        Modprobe modprobe_;
    };

} // namespace android::init

