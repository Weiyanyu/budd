#include "threadPool.h"

using namespace budd;

ThreadPool::ThreadPool(int workerCount)
    : m_stop(false)
{
    for (int i = 0; i < workerCount; i++)
    {
        m_workers.emplace_back([this]() {
            while (true)
            {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->m_tasksMutex);
                    //1. block thread, or else this thread will enter idel state
                    //2. wait condition is this->m_stop == true(quit thread pool) or task is not empty
                    this->m_condition.wait(lock, [this]() {
                        return this->m_stop || !this->m_tasks.empty();
                        });

                    //3. if quit thread poll and taks is empty, it's mean we don't need worker any more, so return
                    if (this->m_stop && this->m_tasks.empty())
                    {
                        return;
                    }

                    //because enqueue is a temp object(right value), so we must use std::move
                    task = std::move(this->m_tasks.front());
                    this->m_tasks.pop();
                }

                //run task
                task();
            }
            });
    }
}

ThreadPool::~ThreadPool()
{
    //1. set m_stop = false
    {
        std::unique_lock<std::mutex> lock(this->m_tasksMutex);
        m_stop = true;
    }

    //2. notify all blocked thread
    m_condition.notify_all();

    //3. wait all working thread stop
    for (std::thread &worker : this->m_workers)
    {
        worker.join();
    }
}

