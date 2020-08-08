#include "eventLoop.h"
#include "channel.h"
#include "selector.h"
#include <iostream>


EventLoop::EventLoop()
    :m_looping(false),
    m_quit(false),
    m_selector(new Selector(this))
{
}

void EventLoop::updateChannel(Channel* ch)
{
    m_selector->updateChannel(ch);
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
    }
    m_looping = false;
}

void EventLoop::quit()
{

    m_quit = true;   
}

EventLoop::~EventLoop() 
{
    //TODO
}