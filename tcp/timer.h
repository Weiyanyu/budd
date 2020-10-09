#ifndef BUDDTIMER
#define BUDDTIMER

#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <functional>
#include <sys/timerfd.h>
#include <memory>
#include <queue>
#include <set>
#include <climits>
#include "channel.h"
#include <atomic>
#include <algorithm>

class EventLoop;
class Timer
{
public:
    typedef std::function<void()> timerCallback;

    static long globalId;

    Timer(time_t when, time_t intervalUsec, timerCallback callback);

    void start();
    void restart();

    time_t getExpiration() { return m_when; }
    bool isRepeat() { return m_repeat; }
    long getId() { return m_id; }

    bool operator<(const Timer &rhs) const
    {
        return m_when < rhs.m_when;
    }

    //no need atomic
    static long generateId() { return globalId++; }

private:
    timerCallback m_timerCallback;

    time_t m_when;
    time_t m_intervalUsec;
    bool m_repeat;
    long m_id;
};

class TimerQueue
{
public:
    typedef std::function<void()> timerCallback;
    static const time_t MICROSECOND_PERSECOND = 1000 * 1000;

    TimerQueue(EventLoop *loop);

    static time_t now();

    long addTimer(timerCallback callback, time_t when, time_t interval);
    void cancelTimer(long id);

    ~TimerQueue();

private:
    void handleRead();
    void setTime(time_t when);
    void resetTime(time_t nowTimeStamp);

    void addTimerInLoop(Timer &&timer);
    bool enqueue(Timer &&timer);

    void findExpired(time_t nowTimeStamp, std::vector<std::pair<time_t, Timer>> &activeTimers);

    void cancelTimerInLoop(long id);
    void cancel(long id);

    void readTimerfd(time_t nowTimeStamp);

    EventLoop *m_loop;
    int m_timerfd;
    Channel m_channel;
    std::set<std::pair<time_t, Timer>> m_timerSet;
    std::vector<std::pair<time_t, Timer>> m_activeTimers;
};

#endif