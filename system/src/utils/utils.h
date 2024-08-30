//
// Created by snownf on 24-7-13.
//

#ifndef WRA_UTILS_H
#define WRA_UTILS_H

#include <unistd.h>
#include <cstring>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <vector>
#include <fcntl.h>
#include <set>

namespace wra {
    int exec(char **argv);

    std::string Basename(std::string_view path);

    bool WriteFully(int fd, const void *data, size_t byte_count);

    bool StartsWith(std::string_view s, std::string_view prefix);

    bool StartsWith(std::string_view s, char prefix);

    bool EndsWith(std::string_view s, char prefix);

    bool EndsWith(std::string_view s, std::string_view prefix);

    std::string Dirname(std::string_view path);

    bool Realpath(const std::string &in, std::string *out);

    bool Readlink(const std::string &in, std::string *out);

    bool ReadFileToString(const std::string &path, std::string *content, bool follow_symlinks);

    bool ReadFileToString(const std::string &path, std::string *out);

    bool make_dir(const std::string &path, mode_t mode);

    bool mkdir_recursive(const std::string &path, mode_t mode);

    std::string Trim(const std::string &in);

    std::vector<std::string> Split(const std::string &basicString, const char *delimiter);

    std::string Join(const std::set<std::string> &set, const char *delimiter);
}

#endif //WRA_UTILS_H
