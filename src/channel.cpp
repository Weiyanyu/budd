#include "channel.h"
#include "eventLoop.h"
#include "selector.h"

#include <iostream>

Channel::Channel(EventLoop* eventLoop, int fd)
    :m_eventLoop(eventLoop),
     m_fd(fd),
     m_events(0),
     m_revents(0)
{

}

void Channel::handleEvents()
{
    std::cout << "handle event" << std::endl;
    if ((m_revents & Selector::ERROR_EVENT) && m_errorCallback != nullptr) {
        m_errorCallback();
    }

    if ((m_revents & Selector::READ_EVENT) && m_readCallback != nullptr) {
        m_readCallback();
    }

    if ((m_revents & Selector::WRITE_EVENT) && m_writeCallback != nullptr) {
        m_readCallback();
    }
}

void Channel::update()
{
    m_eventLoop->updateChannel(this);
}



