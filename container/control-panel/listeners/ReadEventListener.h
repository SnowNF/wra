#ifndef READEVENTLISTENER_H
#define READEVENTLISTENER_H

#include <functional>
#include <string>
#include <thread>
#include <iostream>
#include <fcntl.h>

class ReadEventListener {
public:
    typedef std::function<void()> Trigger;
private:
    Trigger trigger;
    int fd = -1;
    bool running = true;
    std::chrono::steady_clock::time_point lastTri;

public:
    explicit ReadEventListener(Trigger trigger);

    ~ReadEventListener();

    void setPath(const std::string &path);

private:
    void tri();

    void loop();
};


#endif // READEVENTLISTENER_H
