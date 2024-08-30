#include "init/ueventd.h"
#include "logger.h"
#include "main.h"
#include <filesystem>

UEvent_Config uEvent_Config{};

int main(int argc, char **argv) {
    for (int i = 0; i < argc; ++i) {
        std::string arg = std::string(argv[i]);
        wra_info("Find para %s", argv[i]);
        if (arg == "-h" || arg == "--help") {
            char info[] = "Options:\n"
                          "  --debug\n"
                          "  --help -h\n"
                          "  --no-cold-boot\n"
                          "  --no-loop\n";
            wra_info("%s", info);
            return 0;
        }
        if (arg == "--debug")
            uEvent_Config.debug = true;
        if (arg == "--no-cold-boot")
            uEvent_Config.cold_boot = false;
        if (arg == "--no-loop")
            uEvent_Config.loop = false;
    }
    android::init::ueventd_main(argc, argv);
    return 0;
}

