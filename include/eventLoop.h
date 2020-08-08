#ifndef EVENTLOOP
#define EVENTLOOP
#include <memory>
#include <vector>


class Channel;
class Selector;

class EventLoop {
public:

    EventLoop();
    ~EventLoop();

    void updateChannel(Channel* ch);

    void loop();
    void quit();

private:
    bool m_looping;
    bool m_quit;
    std::unique_ptr<Selector> m_selector;
    std::vector<Channel*> m_activeChannelList;
};

#endif