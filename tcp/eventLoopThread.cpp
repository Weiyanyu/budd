#include "eventLoopThread.h"
#include "eventLoop.h"

using namespace budd::tcp;


EventLoopThread::EventLoopThread(const std::string name)
    : m_name(name),
      m_started(false),
      m_eventLoop(nullptr)
{
}

EventLoop *EventLoopThread::start()
{
    //no need lock
    assert(m_started == false);

    m_thread = std::thread(std::bind(&EventLoopThread::threadFunc, this));

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock);
    }
    m_started = true;
    return m_eventLoop;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_eventLoop = &loop;
        m_condition.notify_one();
    }
    loop.loop();
}
