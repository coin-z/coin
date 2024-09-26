/**
 * @file shm_memory.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include <cstdio>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <fstream>
#include <iostream>
#include <map>

#include <coin-data/local/impl/ipcs.hpp>
#include <coin-data/local/impl/buddy.hpp>

namespace coin::data::__inner
{
class ShmMemory
{
public:
    ShmMemory(const ShmMemory&) = delete;
    ShmMemory& operator = (const ShmMemory&) = delete;
    ~ShmMemory();

    static ShmMemory& mem();

    void* malloc(const std::size_t size);
    bool free(void* mem);

    void* obj_malloc(const std::size_t size);
    bool obj_free(void* mem);

    constexpr static std::size_t PAGE_SIZE = 4096;

    void* system_ctrl_addr();
    void* obj_map_addr();
    void* proc_map_addr();

    void initialized();
    void destory();

    class ProcessMutex
    {

    public:
        ProcessMutex();
        ~ProcessMutex();

        bool lock();
        bool unlock();

        int timedlock(int ms);

        inline pid_t holder() const { return holder_pid_; }

    private:
        pthread_mutex_t mutex_;
        pid_t holder_pid_;

    };

    class ProcessRWMutex
    {
    public:
        ProcessRWMutex();
        ~ProcessRWMutex();

        bool rlock();
        bool wlock();
        bool unlock();

    private:
        pthread_rwlock_t rwlock_;
    };

private:
    ShmMemory();

private:
    std::shared_ptr<Shm> buddy_sys_info_;
    std::shared_ptr<Shm> buddy_data_info_;

    std::shared_ptr<Shm> shm_obj_info_;
    std::shared_ptr<Shm> shm_obj_data_info_;
};

} // namespace coin::data::__inner
