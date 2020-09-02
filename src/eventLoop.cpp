#include "eventLoop.h"
#include "channel.h"
#include "selector.h"
#include <sys/eventfd.h>
#include "timer.h"

thread_local EventLoop* threadLocalEventLoop;

IgnoreSigPipe initObj;


int createEventFd()
{
    int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (fd < 0) {
        LOG(FATAL) << "failed to create event fd";
    }
    return fd;
}

EventLoop::EventLoop()
    :m_looping(false),
    m_quit(false),
    m_threadId(std::this_thread::get_id()),
    m_selector(new Selector(this)),
    m_wakeupFd(createEventFd()),
    m_wakeupChannel(new Channel(this, m_wakeupFd)),
    m_callingTask(false),
    m_timerQueue(new TimerQueue(this))
{
    if (threadLocalEventLoop != nullptr) {
        LOG(FATAL) << "one thread only can have on event loop!!!";
    }
    else {
        threadLocalEventLoop = this;
    }

    m_wakeupChannel->registeReadCallback(std::bind(&EventLoop::handleRead, this));
    m_wakeupChannel->enableRead();
}

void EventLoop::updateChannel(Channel* ch)
{
    assert(ch->eventLoop() == this);
    assertInLoopThread();
    m_selector->updateChannel(ch);
}

void EventLoop::removeChannel(Channel* ch)
{
    assert(ch->eventLoop() == this);
    assertInLoopThread();
    m_selector->removeChannel(ch);
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
        execTasks();
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


void EventLoop::runInLoop(const taskFunc& t)
{
    if (isInLoopThread()) {
        t();
    }
    else {
        queueInLoop(t);
    }
}

void EventLoop::queueInLoop(const taskFunc& t)
{
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_taskQ.push_back(t);
    }

    if (!isInLoopThread() || m_callingTask) {
        wakeup();
    }
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    write(m_wakeupFd, &one, sizeof(one));
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    read(m_wakeupFd, &one, sizeof(one));
}


/**
 *  this func will called by multi thread.
 *  so we must use lock to protect.
 *  but just only use lock maybe lead "dead lock"
 *  because task function maybe call queueInLoop
 * */
void EventLoop::execTasks()
{
    m_callingTask = true;
    std::vector<taskFunc> tempTaskQ;
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        tempTaskQ.swap(m_taskQ);
    }

    for (taskFunc t : tempTaskQ) {
        t();
    }

    m_callingTask = false;

}

long EventLoop::runAt(time_t when, timerCallback callback)
{
    return m_timerQueue->addTimer(std::move(callback), when, 0);
}
long EventLoop::runAfter(time_t delay, timerCallback callback)
{
    time_t when = TimerQueue::now() + delay;
    return m_timerQueue->addTimer(std::move(callback), when, 0);
}
long EventLoop::runEvey(time_t interval, timerCallback callback)
{
    time_t when = TimerQueue::now() + interval;
    return m_timerQueue->addTimer(std::move(callback), when, interval);
}

void EventLoop::cancelTimer(long timerId)
{
    m_timerQueue->cancelTimer(timerId);
}