#ifndef EVENTLOOPTHREADPOOL_H
#define EVENTLOOPTHREADPOOL_H

#include <vector>
#include <memory>
#include <cassert>
#include <string>
#include "eventLoopThread.h"

namespace budd 
{
namespace tcp
{
class EventLoop;
class EventLoopThreadPool
{
public:
    EventLoopThreadPool(EventLoop *bootstrapEventLoop, int loopNum);
    void start();
    EventLoop *getNextLoop();

private:
    EventLoop *m_bootstrapEventLoop;

    int m_loopNum;

    std::vector<std::unique_ptr<EventLoopThread>> m_threads;
    std::vector<EventLoop *> m_loops;
    bool m_started;
    size_t m_next;
};
} //namespace tcp
} //namespace budd
#endif