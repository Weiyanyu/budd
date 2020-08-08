#include "eventLoop.h"
#include "channel.h"
#include "selector.h"
#include <cstring>
#include <unistd.h>


#include <sys/timerfd.h>
#include <memory>

#include <iostream>

EventLoop* g_loop;


void timeout()
{
    printf("Timeout!\n");
    g_loop->quit();
}

int main() {
    EventLoop loop;
    g_loop = &loop;

    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(&loop, timerfd);
    channel.registeReadCallback(std::bind(&timeout));
    channel.enableEvents(Selector::READ_EVENT);

    struct itimerspec howlong;
    memset(&howlong, 0, sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    timerfd_settime(timerfd, 0, &howlong, nullptr);
    loop.loop();
    close(timerfd);

}