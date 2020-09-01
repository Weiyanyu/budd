#include "eventLoop.h"
#include "timer.h"


int count = 0;
TimerQueue* globalQ;
int timer2Id;

void printMsg() {
    std::cout << "print msg" << std::endl;
}

void printMsg2() {
    count++;
    std::cout << "print msg2" << std::endl;
    if (count == 5) {
        std::cout << "cancel timer2" << std::endl;
        globalQ->cancelTimer(timer2Id);
    }
    
}



int main()
{
    EventLoop loop;
    TimerQueue q(&loop);
    globalQ = &q;
    q.addTimer(printMsg, TimerQueue::now() + 1 * TimerQueue::MICROSECOND_PERSECOND, 1 * TimerQueue::MICROSECOND_PERSECOND);
    timer2Id = q.addTimer(printMsg2, TimerQueue::now() + 3 * TimerQueue::MICROSECOND_PERSECOND, 3 * TimerQueue::MICROSECOND_PERSECOND);
    
    loop.loop();
    return 0;
}