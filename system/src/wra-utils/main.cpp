#include <string>
#include "DeviceMapper.h"
#include "utils.h"
#include "main.h"
#include <libmodprobe/modprobe.h>

UEvent_Config uEvent_Config{};

int main(int argc, char **argv) {
    if (argc < 2) {
        wra_error("argc < 2");
        return -1;
    }

    auto cmd = std::string(argv[1]);

    if (cmd == "GetDmDevicePathByName") {
        auto dm = DeviceMapper::Instance();
        std::string path;
        dm.GetDmDevicePathByName(argv[2], &path);
        if (argc > 3)
            printf("%s", path.c_str());
        else
            wra_info("DevicePath: %s for Device %s", path.c_str(), argv[1]);
        return 0;
    }

    if (cmd == "FormatPartitionIfWiped") {
        if (DeviceMapper::partition_wiped(argv[2])) {
            wra_info("Partition %s is wiped", argv[2]);
            char f2fsPath[] = "/system/bin/mkfs.f2fs";
            char *f2fs[] = {f2fsPath, argv[2], nullptr};
            wra_info("exec %s %s", f2fsPath, argv[2]);
            wra::exec(f2fs);
        } else {
            wra_info("Partition %s is not wiped", argv[2]);
        }
        return 0;
    }
    if (cmd == "LoadListedModules") {
        std::vector<std::string> basePaths;
        bool useBlocklist = true;
        bool strictMode = false;
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--help" || arg == "-h") {
                wra_info("%s %s [opt]", argv[0], argv[1]);
                wra_info("Available opts are:");
                wra_info("    --base-path : base path for modules");
                wra_info("    --no-use-blocklist : do not use modules.blocklist in base path");
                wra_info("    --strict-mode : quit when one failed");
                wra_info("    --debug : enable debug output");
                return 0;
            }
            if (arg == "--base-path") {
                basePaths.emplace_back(argv[i + 1]);
                i++;
                continue;
            }
            if (arg == "--no-use-blocklist") {
                useBlocklist = false;
                continue;
            }
            if (arg == "--strict-mode") {
                strictMode = true;
                continue;
            }
            if (arg == "--debug") {
                uEvent_Config.debug = true;
                continue;
            }
        }
        Modprobe modprobe(basePaths, useBlocklist);
        modprobe.LoadListedModules(strictMode);
        return 0;
    }
    wra_error("Unknown cmd %s", cmd.c_str());
    wra_error("Available cmds are:");
    wra_error("   GetDmDevicePathByName");
    wra_error("   FormatPartitionIfWiped");
    wra_error("   LoadListedModules");
    return 1;
}