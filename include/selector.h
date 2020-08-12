#ifndef SELECTOR_H
#define SELECTOR_H

#include <sys/epoll.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <algorithm>
#include <iostream>

class EventLoop;
class Channel;

class Selector {
public:
    static const int READ_EVENT = EPOLLIN | EPOLLPRI;
    static const int WRITE_EVENT = EPOLLOUT;
    static const int ERROR_EVENT = EPOLLERR;
    static const int EMPTY_EVENT = 0;

    Selector(EventLoop* m_eventLoop);

    void select(int timeout, std::vector<Channel*> &activeChannels);
    void updateChannel(Channel* ch);
    void removeChannel(Channel* ch);

private:
    EventLoop* m_eventLoop;
    std::unordered_map<int, Channel*> m_channelMaps;
    std::vector<struct epoll_event> m_reventsList;

    int m_epollfd;

};
#endif