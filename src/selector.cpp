#include "selector.h"
#include "channel.h"
#include <cassert>
#include <algorithm>
#include <iostream>

Selector::Selector(EventLoop* eventLoop)
    :m_eventLoop(eventLoop)
{
    m_epollfd = epoll_create(1024);
    m_reventsList.resize(1024);
}

void Selector::updateChannel(Channel* ch)
{
    int fd = ch->fd();

    struct epoll_event newEvent;
    newEvent.data.fd = fd;
    newEvent.events = ch->events();
    if (m_channelMaps.count(fd) == 0) {
        //new channel
        epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &newEvent);
        m_channelMaps[fd] = ch;
    } else {
        //old channel
        if (ch->events() == EMPTY_EVENT) {
            epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, &newEvent);
            m_channelMaps.erase(fd);
        } else {
            epoll_ctl(m_epollfd, EPOLL_CTL_MOD, fd, &newEvent);
        }
    }
}

void Selector::select(int timeout, std::vector<Channel*> &activeChannels)
{
    int eventNum = epoll_wait(m_epollfd, m_reventsList.data(), m_reventsList.size(), timeout);
    if (eventNum > 0) {
        std::cout << eventNum << " events happend" << std::endl;
        for (int i = 0; i < eventNum; i++) {
            int fd = m_reventsList[i].data.fd;
            auto channelIter = m_channelMaps.find(fd);
            assert(channelIter != m_channelMaps.end());
            auto channel = channelIter->second;
            channel->setRevents(m_reventsList[i].events);
            activeChannels.push_back(channel);
        }

    } else if (eventNum == 0) {
        std::cout << "not event in this time" << std::endl;
    } else {
        std::cerr << "select error!!!" << std::endl;
    }

}