//
// Created by snownf on 24-7-16.
//

#ifndef WRA_MAIN_H
#define WRA_MAIN_H

#include <string>
#include <vector>
#include <set>

struct UEvent_Config {
    bool debug = false;
    bool cold_boot = true;
    bool loop = true;
};

extern UEvent_Config uEvent_Config;

#endif //WRA_MAIN_H
