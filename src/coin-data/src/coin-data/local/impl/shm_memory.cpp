/**
 * @file shm_memory.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "impl/shm_memory.hpp"
#include "shm_memory.hpp"

#include <time.h>
#include <sys/types.h>
#include <unistd.h>

namespace coin::data::__inner
{
constexpr static uint64_t SHM_ADDR_DATA_INFO_SIZE = ShmMemory::PAGE_SIZE;
constexpr static uint64_t SHM_ADDR_DATA_INFO = (0x7f1fff000000);

constexpr static uint64_t SHM_ADDR_DATA_SIZE = ((uint64_t)0x1 << 32);
constexpr static uint64_t SHM_ADDR_DATA = (0x7f2fff000000);

constexpr static uint64_t SHM_ADDR_SYSTEM_INFO_SIZE = (ShmMemory::PAGE_SIZE * 6);
constexpr static uint64_t SHM_ADDR_SYSTEM_INFO = (0x7fdfff000000);

constexpr static uint64_t SHM_ADDR_SYSTEM_DATA_SIZE = ((uint64_t)0x1 << 24);
constexpr static uint64_t SHM_ADDR_SYSTEM_DATA = (0x7fcfff000000);

ShmMemory::ShmMemory()
  : buddy_sys_info_(std::make_shared<Shm>("coin-data-buddy-system.shm", (void*)SHM_ADDR_DATA_INFO, SHM_ADDR_DATA_INFO_SIZE))
  , buddy_data_info_(std::make_shared<Shm>("coin-data-buddy-data.shm", (void*)SHM_ADDR_DATA, SHM_ADDR_DATA_SIZE))
  , shm_obj_info_(std::make_shared<Shm>("coin-data-obj-system.shm", (void*)SHM_ADDR_SYSTEM_INFO, SHM_ADDR_SYSTEM_INFO_SIZE))
  , shm_obj_data_info_(std::make_shared<Shm>("coin-data-obj-data.shm", (void*)SHM_ADDR_SYSTEM_DATA, SHM_ADDR_SYSTEM_DATA_SIZE))
{
}

ShmMemory::~ShmMemory()
{

}

ShmMemory& ShmMemory::mem()
{
    static ShmMemory sm;
    return sm;
}

void *ShmMemory::malloc(const std::size_t size)
{
    return buddy_malloc((struct buddy_head_t* )buddy_sys_info_->addr(), size);
}

bool ShmMemory::free(void *mem)
{
    return (0 == buddy_free((struct buddy_head_t* )buddy_sys_info_->addr(), mem));
}

void *ShmMemory::obj_malloc(const std::size_t size)
{
    return buddy_malloc((struct buddy_head_t* )shm_obj_info_->addr(), size);
}

bool ShmMemory::obj_free(void *mem)
{
    return (0 == buddy_free((struct buddy_head_t* )shm_obj_info_->addr(), mem));
}

void *ShmMemory::system_ctrl_addr()
{
    return (void*)((uint8_t*)shm_obj_info_->addr() + PAGE_SIZE);
}

void *ShmMemory::obj_map_addr()
{
    return (void*)((uint8_t*)shm_obj_info_->addr() + (PAGE_SIZE * 2));
}

void *ShmMemory::proc_map_addr()
{
    return (void*)((uint8_t*)shm_obj_info_->addr() + (PAGE_SIZE * 3));
}

void ShmMemory::initialized()
{
    // 初始化 buddy 系统
    if(0 > buddy_init_system(buddy_sys_info_->addr(), buddy_sys_info_->size()))
    {
        fprintf(stderr, "sys info buddy init system failed.\n");
        exit(-1);
    }

    // 挂载数据内存块
    if(0 > buddy_attch_memory((struct buddy_head_t* )buddy_sys_info_->addr(), buddy_data_info_->addr(), buddy_data_info_->size()))
    {
        fprintf(stderr, "buddy attach memory failed.\n");
        exit(-1);
    }

    // 初始化 buddy 系统
    if(0 > buddy_init_system(shm_obj_info_->addr(), PAGE_SIZE))
    {
        fprintf(stderr, "buddy init system failed.\n");
        exit(-1);
    }

    // 挂载数据内存块
    if(0 > buddy_attch_memory((struct buddy_head_t* )shm_obj_info_->addr(), shm_obj_data_info_->addr(), shm_obj_data_info_->size()))
    {
        fprintf(stderr, "buddy attach memory failed.\n");
        exit(-1);
    }
}

void ShmMemory::destory()
{
    buddy_sys_info_.reset();
    buddy_data_info_.reset();
    shm_obj_info_.reset();
    shm_obj_data_info_.reset();
}

ShmMemory::ProcessMutex::ProcessMutex() : holder_pid_(0)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&mutex_, &attr);

    pthread_mutex_unlock(&mutex_);
}

ShmMemory::ProcessMutex::~ProcessMutex()
{
    if(0 != pthread_mutex_trylock(&mutex_))
    {
        pthread_mutex_unlock(&mutex_);
    }

    pthread_mutex_destroy(&mutex_);
}

bool ShmMemory::ProcessMutex::lock()
{
    if(0 != pthread_mutex_lock(&mutex_))
    {
        return false;
    }
    holder_pid_ = getpid();
    return true;
}

bool ShmMemory::ProcessMutex::unlock()
{
    if(0 != pthread_mutex_unlock(&mutex_))
    {
        abort();
        return false;
    }
    return true;
}

int ShmMemory::ProcessMutex::timedlock(int ms)
{
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    uint64_t now = timeout.tv_sec * 1000 * 1000 * 1000 + timeout.tv_nsec + (ms * 1000);

    timeout.tv_sec = now / 1000 / 1000 / 1000;
    timeout.tv_nsec = now % (1000 * 1000 * 1000);
    int ret = pthread_mutex_timedlock(&mutex_, &timeout);
    if(ret == 0)
    {
        holder_pid_ = getpid();
    }
    else if(ret == ETIMEDOUT)
    {

    }
    else
    {
        fprintf(stderr, "Unknown error(%d), %ld, %ld.\n", ret, timeout.tv_sec, timeout.tv_nsec);
        abort();
    }
    return ret;
}

ShmMemory::ProcessRWMutex::ProcessRWMutex()
{
    pthread_rwlockattr_t attr;
    pthread_rwlockattr_init(&attr);
    pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_rwlock_init(&rwlock_, &attr);
}

ShmMemory::ProcessRWMutex::~ProcessRWMutex()
{
    pthread_rwlock_destroy(&rwlock_);
}

bool ShmMemory::ProcessRWMutex::rlock()
{
    if(0 != pthread_rwlock_rdlock(&rwlock_))
    {
        return false;
    }
    return true;
}

bool ShmMemory::ProcessRWMutex::wlock()
{
    if(0 != pthread_rwlock_wrlock(&rwlock_))
    {
        return false;
    }
    return true;
}

bool ShmMemory::ProcessRWMutex::unlock()
{
    if(0 != pthread_rwlock_unlock(&rwlock_))
    {
        return false;
    }
    return true;
}
} // namespace coin::data::__inner
