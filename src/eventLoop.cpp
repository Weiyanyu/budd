#include "eventLoop.h"
#include "channel.h"
#include "selector.h"

thread_local EventLoop* threadLocalEventLoop;

EventLoop::EventLoop()
    :m_looping(false),
    m_quit(false),
    m_threadId(std::this_thread::get_id()),
    m_selector(new Selector(this))
{
    if (threadLocalEventLoop != nullptr) {
        LOG(FATAL) << "one thread only can have on event loop!!!";
    } else {
        threadLocalEventLoop = this;
    }
}

void EventLoop::updateChannel(Channel* ch)
{
    assert(ch->eventLoop() == this);
    assertInLoopThread();
    m_selector->updateChannel(ch);
}

void EventLoop::loop() 
{
    m_looping = true;
    while (m_quit == false) {

        m_activeChannelList.clear();
        m_selector->select(-1, m_activeChannelList);
        for (auto channel : m_activeChannelList) {
            channel->handleEvents();
        }
    }
    m_looping = false;
}

void EventLoop::quit()
{
    m_quit = true;   
}

void EventLoop::assertInLoopThread() 
{
    if (m_threadId != std::this_thread::get_id()) {
        LOG(FATAL) << "one thread only can have on event loop!!!";
    }
}

EventLoop::~EventLoop() 
{
    threadLocalEventLoop = nullptr;
}
EventLoop* EventLoop::getEventLoopInThread()
{
    assertInLoopThread();
    return threadLocalEventLoop;
}
