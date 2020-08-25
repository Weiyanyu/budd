#include "eventLoopThreadPool.h"
#include "eventLoop.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* bootstrapEventLoop, int loopNum)
    :m_bootstrapEventLoop(bootstrapEventLoop),
     m_loopNum(loopNum),
     m_threads(loopNum),
     m_loops(loopNum),
     m_started(false),
     m_next(0)
{

}

void EventLoopThreadPool::start()
{
    assert(m_started == false);
    m_bootstrapEventLoop->assertInLoopThread();
    for (int i = 0; i < m_loopNum; i++) {
        std::string loopName = "loop_" + std::to_string(i);
        EventLoopThread* et = new EventLoopThread(loopName);
        m_threads[i] = std::unique_ptr<EventLoopThread>(et);
        m_loops[i] = et->start();
    }
    m_started = true;
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    assert(m_started);
    m_bootstrapEventLoop->assertInLoopThread();
    EventLoop* loop = m_bootstrapEventLoop;
    if (m_loops.empty()) {
        return loop;
    }
    loop = m_loops[m_next++];
    
    if (m_next >= m_loops.size()) {
        m_next = 0;
    }
    return loop;
}