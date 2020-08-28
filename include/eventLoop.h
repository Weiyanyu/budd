#ifndef EVENTLOOP
#define EVENTLOOP
#include <memory>
#include <vector>
#include <iostream>
#include <glog/logging.h>
#include <thread>
#include <mutex>
#include <functional>
#include <signal.h>

class Channel;
class Selector;

class EventLoop {
public:

    using taskFunc = std::function<void()>;

    EventLoop();
    ~EventLoop();

    void updateChannel(Channel* ch);
    void removeChannel(Channel* ch);


    void loop();
    void quit();
    bool isInLoopThread() {
        return std::this_thread::get_id() == m_threadId;
    }

    void assertInLoopThread();

    EventLoop* getEventLoopInThread();
    void runInLoop(const taskFunc& t);
    void queueInLoop(const taskFunc& t);

    void wakeup();

    std::thread::id getCurrentThreadId() { return m_threadId; }
private:
    void handleRead();
    void execTasks();

    bool m_looping;
    bool m_quit;
    std::thread::id m_threadId;
    std::unique_ptr<Selector> m_selector;
    std::vector<Channel*> m_activeChannelList;

    std::vector<taskFunc> m_taskQ;
    std::mutex m_mutex;
    int m_wakeupFd;
    std::unique_ptr<Channel> m_wakeupChannel;

    bool m_callingTask;
};

class IgnoreSigPipe
{
public:
    IgnoreSigPipe()
    {
        ::signal(SIGPIPE, SIG_IGN);
    }
};


#endif