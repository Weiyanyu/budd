#ifndef BUDD_THREADPOOL_H
#define BUDD_THREADPOOL_H

#include <vector>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <future>
#include <type_traits>
#include <memory>
#include <stdexcept>

namespace budd
{
    class ThreadPool
    {
    public:
        ThreadPool(int workerCount);
        ~ThreadPool();

        template <typename F, typename... Args>
        auto enqueue(F &&f, Args &&... args)
            -> std::future<typename std::result_of<F(Args...)>::type>;

    private:
        //worker list
        std::vector<std::thread> m_workers;

        //task list
        std::queue<std::function<void()>> m_tasks;

        std::mutex m_tasksMutex;
        std::condition_variable m_condition;

        bool m_stop;
    };

    //Note: template function implation need in header file(but don't have to)
    template <typename F, typename... Args>
    auto ThreadPool::enqueue(F &&f, Args &&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>
    {
        typedef typename std::result_of<F(Args...)>::type return_type;

        //construct task
        //1. use bind, it can bring up callable object and args, and than return function object
        //2. than function object as args pass to packaged_task
        //3. so, task's type is packaged_task
        //4. why need packaged_task? we want async(future) and thread execution function have return value.
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(this->m_tasksMutex);
            if (m_stop)
            {
                throw std::logic_error("threadpoll already stop!!!");
            }

            this->m_tasks.emplace([task]() {
                //Note: This is where the execution function is actually called
                (*task)();
            });
        }
        m_condition.notify_one();
        //return future, caller can wait and get result
        return res;
    }

} // namespace budd

#endif