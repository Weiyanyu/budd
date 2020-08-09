#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>
#include <memory>
#include <iostream>
#include <glog/logging.h>

class EventLoop;

class Channel {
public:
    typedef std::function<void()> eventCallback;

    Channel(EventLoop* eventLoop, int fd);

    void registeReadCallback(eventCallback callback) { m_readCallback = std::move(callback); }
    void registeWriteCallback(eventCallback callback) { m_writeCallback = std::move(callback); }
    void registeErrorCallback(eventCallback callback) { m_errorCallback = std::move(callback); }

    void enableEvents(int events) { m_events |= events; update(); }
    void disableEvents(int events) { m_events &= ~events; update(); }

    void handleEvents();

    //getter and setter
    int fd() { return m_fd; }
    int events() { return m_events; }
    int revents() { return m_revents; }
    void setRevents(int revents) { m_revents = revents; }
    EventLoop* eventLoop() { return m_eventLoop; }


private:
    EventLoop* m_eventLoop;

    int m_fd;
    int m_events;
    int m_revents;

    eventCallback m_readCallback;
    eventCallback m_writeCallback;
    eventCallback m_errorCallback;

    void update();

};

#endif