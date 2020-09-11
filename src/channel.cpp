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
    // if ((m_revents & Selector::HUP_EVENT) && !(m_revents & Selector::READ_EVENT)) {
    //     if (m_closeCallback != nullptr) {
    //         LOG(INFO) << "close event!!!!!!!!!!!!!!!!!!!!!!!";
    //         m_closeCallback();
    //     }
    // }

    if ((m_revents & Selector::ERROR_EVENT) && m_errorCallback != nullptr) {
        DLOG(INFO) << "error events";
        m_errorCallback();
    }

    if ((m_revents & Selector::READ_EVENT) && m_readCallback != nullptr) {
        DLOG(INFO) << "read events";

        m_readCallback();
    }

    if ((m_revents & Selector::WRITE_EVENT) && m_writeCallback != nullptr) {
        DLOG(INFO) << "write events";
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



