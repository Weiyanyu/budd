#ifndef EVENTLOOP
#define EVENTLOOP
#include <memory>
#include <vector>
#include <iostream>
#include <glog/logging.h>
#include <thread>
#include <mutex>

class Channel;
class Selector;

class EventLoop {
public:

    EventLoop();
    ~EventLoop();

    void updateChannel(Channel* ch);
    void removeChannel(Channel* ch);


    void loop();
    void quit();
    bool isInLoopThread() { return std::this_thread::get_id() == m_threadId; }

    void assertInLoopThread();

    EventLoop* getEventLoopInThread();


private:
    bool m_looping;
    bool m_quit;
    std::thread::id m_threadId;
    std::unique_ptr<Selector> m_selector;
    std::vector<Channel*> m_activeChannelList;
};

#endif