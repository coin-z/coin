/**
 * @file event.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "utils/event.hpp"
#include "event.hpp"
#include <asio.hpp>

namespace coin
{
EventPool::EventPool() : thread_pool_( std::make_shared<asio::thread_pool>(6) )
{
}
EventPool::~EventPool()
{
    thread_pool_->join();
}
void EventPool::run_and_clear_all_ready_task()
{
    std::lock_guard<std::mutex> lock(ready_task_deque_mutex_);
    for(auto it = ready_task_deque_.begin(); it != ready_task_deque_.end(); ++it)
    {
        asio::post(*thread_pool_, std::bind(&TaskBase::invoke, (*it).get()));
    }
    ready_task_deque_.clear();
}
void EventPool::clear_all_ready_task()
{
    std::lock_guard<std::mutex> lock(ready_task_deque_mutex_);
    ready_task_deque_.clear();
}
void EventPool::spin_once()
{
    std::lock_guard<std::mutex> lock(event_deque_mutex_);
    for(auto it = event_deque_.begin(); it != event_deque_.end(); ++it)
    {
        if(it->first->operator bool())
        {
            std::lock_guard<std::mutex> ready_task_lock(ready_task_deque_mutex_);
            ready_task_deque_.push_back(it->second);
        }
    }
}
} // namespace coin
