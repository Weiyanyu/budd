#include "selector.h"
#include "channel.h"


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
            Channel *ch = m_channelMaps[fd];
            //TODO: for temp, can't delete here
            delete ch;
            m_channelMaps.erase(fd);
            close(fd);
        } else {
            epoll_ctl(m_epollfd, EPOLL_CTL_MOD, fd, &newEvent);
        }
    }
}

void Selector::select(int timeout, std::vector<Channel*> &activeChannels)
{
    int eventNum = epoll_wait(m_epollfd, m_reventsList.data(), m_reventsList.size(), timeout);
    if (eventNum > 0) {
        LOG(INFO) << eventNum << " events happend";
        for (int i = 0; i < eventNum; i++) {
            int fd = m_reventsList[i].data.fd;
            auto channelIter = m_channelMaps.find(fd);
            assert(channelIter != m_channelMaps.end());
            auto channel = channelIter->second;
            channel->setRevents(m_reventsList[i].events);
            activeChannels.push_back(channel);
        }

    } else if (eventNum == 0) {
        LOG(INFO)  << "not event in this time";
    } else {
        LOG(ERROR) << "select error!!!";
    }

}