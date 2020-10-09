#include "timer.h"
#include "eventLoop.h"

int createTimerfd()
{
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                   TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0)
    {
        LOG(FATAL) << "Failed in timerfd_create";
    }
    return timerfd;
}

long Timer::globalId = 0;

Timer::Timer(time_t when, time_t intervalUsec, timerCallback callback)
    : m_timerCallback(std::move(callback)),
      m_when(when),
      m_intervalUsec(intervalUsec),
      m_repeat(intervalUsec > 0),
      m_id(generateId())
{
}

void Timer::start()
{
    LOG(INFO) << "timer(" << m_id << ") timeout";
    m_timerCallback();
}

void Timer::restart()
{
    m_when += m_intervalUsec;
}

//-------------------- timerQueue ----------------//
TimerQueue::TimerQueue(EventLoop *loop)
    : m_loop(loop),
      m_timerfd(createTimerfd()),
      m_channel(loop, m_timerfd)
{
    m_channel.registeReadCallback(std::bind(&TimerQueue::handleRead, this));
    m_channel.enableRead();
}

TimerQueue::~TimerQueue()
{
    m_channel.disableEvents();
    m_channel.remove();

    close(m_timerfd);
}

void TimerQueue::handleRead()
{
    m_loop->assertInLoopThread();
    time_t nowTimeStamp = now();
    m_activeTimers.clear();
    readTimerfd(nowTimeStamp);
    LOG(INFO) << "nowTimeSamp = " << nowTimeStamp;
    LOG(INFO) << "activeTimer size init = " << m_activeTimers.size();

    findExpired(nowTimeStamp, m_activeTimers);

    for (auto &timer : m_activeTimers)
    {
        LOG(INFO) << "timer when : " << timer.second.getExpiration();
        timer.second.start();
    }
    //重新把repete timer insert到队列里，且启动队列中的下一个timer的定时器
    resetTime(nowTimeStamp);
}

long TimerQueue::addTimer(timerCallback callback, time_t when, time_t interval)
{
    Timer timer(when, interval, callback);
    auto timerMoved = std::bind(static_cast < Timer && (&)(Timer &) > (std::move<Timer &>), timer);
    m_loop->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timerMoved));
    return timer.getId();
}

void TimerQueue::addTimerInLoop(Timer &&timer)
{
    m_loop->assertInLoopThread();
    bool resetTimeFlag = enqueue(std::forward<Timer>(timer));
    if (resetTimeFlag)
    {
        setTime(timer.getExpiration());
    }
}

bool TimerQueue::enqueue(Timer &&timer)
{
    time_t nowTimeStamp = now();
    assert(nowTimeStamp < timer.getExpiration());

    bool resetLatestTime = false;

    if (m_timerSet.size() == 1 || timer.getExpiration() < m_timerSet.begin()->first)
    {
        resetLatestTime = true;
    }

    m_timerSet.insert(std::make_pair<time_t, Timer>(timer.getExpiration(), std::forward<Timer>(timer)));

    return resetLatestTime;
}

time_t TimerQueue::now()
{
    struct timeval tick;
    gettimeofday(&tick, NULL);
    return tick.tv_sec * MICROSECOND_PERSECOND + tick.tv_usec;
}

void TimerQueue::findExpired(time_t nowTimeStamp, std::vector<std::pair<time_t, Timer>> &activeTimers)
{
    Timer duumyTimer(LONG_MAX, 0, []() {});
    std::pair<time_t, Timer> duumy(nowTimeStamp, duumyTimer);
    auto it = m_timerSet.lower_bound(duumy);
    assert(it == m_timerSet.end() || nowTimeStamp < it->first);
    std::copy(m_timerSet.begin(), it, std::back_inserter(activeTimers));
    m_timerSet.erase(m_timerSet.begin(), it);
}

void TimerQueue::setTime(time_t when)
{
    time_t timerMicroSecond = when - now();
    if (timerMicroSecond < 100)
    {
        timerMicroSecond = 100;
    }

    struct itimerspec newValue;
    newValue.it_value.tv_sec = timerMicroSecond / MICROSECOND_PERSECOND;
    newValue.it_value.tv_nsec = (timerMicroSecond % MICROSECOND_PERSECOND) * 1000;

    newValue.it_interval.tv_sec = 0;
    newValue.it_interval.tv_nsec = 0;

    int ret = timerfd_settime(m_timerfd, 0, &newValue, NULL);
    if (ret)
    {
        LOG(ERROR) << "timerfd settime error!!";
    }
}

void TimerQueue::resetTime(time_t nowTimeStamp)
{
    for (auto &timer : m_activeTimers)
    {
        if (timer.second.isRepeat())
        {
            timer.second.restart();
            enqueue(std::move(timer.second));
        }
    }

    time_t nextWhen = 0;
    if (!m_timerSet.empty())
    {
        auto timer = m_timerSet.begin()->second;
        nextWhen = timer.getExpiration();
    }
    if (nextWhen != 0)
    {
        setTime(nextWhen);
    }
}

void TimerQueue::cancelTimer(long id)
{
    m_loop->queueInLoop(std::bind(&TimerQueue::cancelTimerInLoop, this, id));
}
void TimerQueue::cancelTimerInLoop(long id)
{
    m_loop->assertInLoopThread();
    cancel(id);
}

void TimerQueue::cancel(long timerId)
{
    auto it = std::find_if(m_timerSet.begin(), m_timerSet.end(), [timerId](const std::pair<time_t, Timer> arg) {
        Timer timer = arg.second;
        return timer.getId() == timerId;
    });
    if (it == m_timerSet.end())
    {
        LOG(INFO) << "can't found timer " << timerId << " , please check again";
        return;
    }
    m_timerSet.erase(it);
}

void TimerQueue::readTimerfd(time_t nowTimeStamp)
{
    uint64_t howmany;
    ssize_t n = ::read(m_timerfd, &howmany, sizeof howmany);
    if (n != sizeof howmany)
    {
        LOG(ERROR) << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
    }
}
