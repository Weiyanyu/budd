#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <thread>
#include <string>
#include <cassert>
#include <mutex>
#include <condition_variable>

namespace budd 
{
namespace tcp
{
class EventLoop;
class EventLoopThread
{
public:
    EventLoopThread(const std::string name);
    EventLoop *start();
    EventLoop *getEventLoop() { return m_eventLoop; }
    std::string getName() { return m_name; }

private:
    void threadFunc();

    std::string m_name;
    bool m_started;
    std::thread m_thread;
    EventLoop *m_eventLoop;

    std::mutex m_mutex;
    std::condition_variable m_condition;
};
} //namespace tcp
} //namespace budd
#endif