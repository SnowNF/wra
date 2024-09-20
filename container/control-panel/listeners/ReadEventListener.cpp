#include "ReadEventListener.h"

ReadEventListener::ReadEventListener(std::function<void()> trigger) : trigger(std::move(trigger)) {
    std::thread([this]() {
        loop();
    }).detach();
}

ReadEventListener::~ReadEventListener() {
    running = false;
}

void ReadEventListener::tri() {
    auto now = std::chrono::steady_clock::now();
    if ((now - lastTri) > std::chrono::seconds(1)) {
        lastTri = now;
        trigger();
    }
}

void ReadEventListener::loop() {
    while (running) {
        char buf[1024];
        ssize_t r = ::read(fd, buf, sizeof(buf));
        if (r == -1) {
            std::cerr << "ReadEventListener:Waiting!\n";
            perror("ReadEventListener");
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        tri();
    }
}

void ReadEventListener::setPath(const std::string &path) {
    ::close(fd);
    fd = ::open(path.c_str(), O_RDONLY);
}
