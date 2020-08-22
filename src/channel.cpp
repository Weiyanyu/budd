#include "channel.h"
#include "eventLoop.h"
#include "selector.h"


Channel::Channel(EventLoop* eventLoop, int fd)
    :m_eventLoop(eventLoop),
     m_fd(fd),
     m_events(0),
     m_revents(0)
{

}

void Channel::handleEvents()
{
    if ((m_revents & Selector::HUP_EVENT) && !(m_revents & Selector::READ_EVENT)) {
        if (m_closeCallback != nullptr) {
            LOG(INFO) << "handle close";
            m_closeCallback();
        }
    }

    if ((m_revents & Selector::ERROR_EVENT) && m_errorCallback != nullptr) {
        LOG(INFO) << "handle error";
        m_errorCallback();
    }

    if ((m_revents & Selector::READ_EVENT) && m_readCallback != nullptr) {
        LOG(INFO) << "handle read";
        m_readCallback();
    }

    if ((m_revents & Selector::WRITE_EVENT) && m_writeCallback != nullptr) {
        LOG(INFO) << "handle write";
        m_writeCallback();
    }
}

void Channel::update()
{
    m_eventLoop->updateChannel(this);
}

void Channel::remove()
{
    m_eventLoop->assertInLoopThread();
    m_eventLoop->removeChannel(this);
}



