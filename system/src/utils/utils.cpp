//
// Created by snownf on 24-7-13.
//

#include "utils.h"
#include "../utils/logger.h"
#include <cstdint>
#include <cstddef>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <filesystem>

namespace wra {
    int exec(char **argv) {
        pid_t pid = fork();
        if (pid < 0) {
            wra_error("Fork failed");
            return -1;
        } else if (pid == 0) {
            // Child process
            if (execvp(argv[0], argv) < 0) {
                wra_error("Exec failed");
                return -1;
            }
        } else {
            // Parent process
            int status;
            if (waitpid(pid, &status, 0) < 0) {
                wra_error("Waitpid failed");
                return -1;
            }

            // Return the exit status of the child process
            if (WIFEXITED(status)) {
                return WEXITSTATUS(status);
            } else {
                wra_error("WIFEXITED() failed");
                return -1;
            }
        }

        wra_error("Should never reach here");
        return 0;
    }

    std::string Basename(std::string_view path) {
        size_t pos = path.find_last_of('/');
        if (pos == std::string_view::npos) {
            return std::string(path);
        }
        return std::string(path.substr(pos + 1));
    }

    bool WriteFully(int fd, const void *data, size_t byte_count) {
        const auto *p = reinterpret_cast<const uint8_t *>(data);
        size_t remaining = byte_count;
        while (remaining > 0) {
            ssize_t n = TEMP_FAILURE_RETRY(::write(fd, p, remaining));
            if (n == -1) return false;
            p += n;
            remaining -= n;
        }
        return true;
    }


    bool StartsWith(std::string_view s, std::string_view prefix) {
        return s.substr(0, prefix.size()) == prefix;
    }

    bool StartsWith(std::string_view s, char prefix) {
        return !s.empty() && s.front() == prefix;
    }

    bool EndsWith(std::string_view s, char prefix) {
        return s.ends_with(prefix);
    }

    bool EndsWith(std::string_view s, std::string_view prefix) {
        return s.ends_with(prefix);
    }

    std::string Dirname(std::string_view path) {
        if (path.empty()) {
            return ".";
        }

        size_t pos = path.rfind('/');
        if (pos == std::string_view::npos) {
            return ".";
        }
        if (pos == 0) {
            return "/";
        }

        return std::string(path.substr(0, pos));
    }

    bool Realpath(const std::string &in, std::string *out) {
        char *r = realpath(in.c_str(), nullptr);
        if (r != nullptr) {
            *out = std::string(r); // 将解析后的路径存储到 out 中
            free(r);
            return true;
        } else {
            return false;
        }
    }

    bool ReadFileToString(const std::string &path, std::string *content, bool follow_symlinks) {
        if (content == nullptr) {
            return false;
        }

        std::filesystem::path file_path(path);

        if (!follow_symlinks && std::filesystem::is_symlink(file_path)) {
            return false;
        }
        return ReadFileToString(path, content);
    }

    bool ReadFileToString(const std::string &path, std::string *out) {
        if (out == nullptr) {
            return false;
        }

        std::ifstream file(path);
        if (!file.is_open()) {
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        *out = buffer.str();

        return true;
    }

    bool Readlink(const std::string &in, std::string *out) {
        // Most Linux file systems (ext2 and ext4, say) limit symbolic links to
        // 4095 bytes. Since we'll copy out into the string anyway, it doesn't
        // waste memory to just start there. We add 1 so that we can recognize
        // whether it actually fit (rather than being truncated to 4095).
        std::vector<char> buf(4095 + 1);
        while (true) {
            ssize_t size = readlink(in.c_str(), &buf[0], buf.size());
            // Unrecoverable error?
            if (size == -1) return false;
            // It fit! (If size == buf.size(), it may have been truncated.)
            if (static_cast<size_t>(size) < buf.size()) {
                out->assign(&buf[0], size);
                return true;
            }
            // Double our buffer and try again.
            buf.resize(buf.size() * 2);
        }
    }

    bool mkdir_recursive(const std::string &path, mode_t mode) {
        std::string::size_type slash = 0;
        while ((slash = path.find('/', slash + 1)) != std::string::npos) {
            auto directory = path.substr(0, slash);
            struct stat info{};
            if (stat(directory.c_str(), &info) != 0) {
                auto ret = make_dir(directory, mode);
                if (!ret && errno != EEXIST) return false;
            }
        }
        auto ret = make_dir(path, mode);
        if (!ret && errno != EEXIST) return false;
        return true;
    }

    bool make_dir(const std::string &path, mode_t mode) {
        int rc = mkdir(path.c_str(), mode);
        return rc == 0;
    }

    std::string Trim(const std::string &in) {
        size_t start = 0;
        size_t end = in.length();
        while (start < end && std::isspace(in[start])) {
            ++start;
        }
        while (end > start && std::isspace(in[end - 1])) {
            --end;
        }
        return in.substr(start, end - start);
    };

    std::vector<std::string> Split(const std::string &basicString, const char *delimiter) {
        std::vector<std::string> result;
        size_t start = 0;
        size_t end = 0;

        while ((end = basicString.find(delimiter, start)) != std::string::npos) {
            result.push_back(basicString.substr(start, end - start));
            start = end + std::strlen(delimiter);
        }
        result.push_back(basicString.substr(start));

        return result;
    }

    std::string Join(const std::set<std::string> &set, const char *delimiter) {
        std::string result;
        for (auto it = set.begin(); it != set.end(); ++it) {
            if (it != set.begin()) {
                result += delimiter;
            }
            result += *it;
        }
        return result;
    }
}