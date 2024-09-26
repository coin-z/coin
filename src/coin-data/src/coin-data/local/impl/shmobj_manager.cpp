/**
 * @file shmobj_manager.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <signal.h>
#include <sys/types.h>

#include <coin-commons/utils/crc32.hpp>
#include "local/impl/shmobj_manager.hpp"
#include "shmobj_manager.hpp"

namespace coin::data
{

ShmObjManager::ShmObjManager()
  : self_pid_(getpid())
  , key_file_lock_("/tmp/coin-data-key-file.lock")
{
    static_assert(sizeof(ShmMemSystemControl) <= __inner::ShmMemory::PAGE_SIZE, "sizeof(ShmMemSystemControl) <= PAGE_SIZE");

    auto initialized = [this]{
        __inner::ShmMemory::mem().initialized();
        shm_mem_sys_control_ = new (shm_mem_sys_control_)struct ShmMemSystemControl;
    };
    
    {
        // 加锁
        FileLockGuard lock(key_file_lock_);

        // 挂接到系统控制
        shm_mem_sys_control_ = (struct ShmMemSystemControl*)((uint8_t*)__inner::ShmMemory::mem().system_ctrl_addr());
        
        // 检查是否已经初始化，如未进行初始化，则进行初始化
        if(shm_mem_sys_control_->magic_value != shm_system_magic_key)
        {
            uint64_t rec_crc = shm_mem_sys_control_->crc;
            shm_mem_sys_control_->crc = 0;
            uint64_t crc = coin::calculate_crc32(0, (const unsigned char*)shm_mem_sys_control_, sizeof(ShmMemSystemControl));
            if(shm_mem_sys_control_->crc == 0 || crc != rec_crc)
            {
                fprintf(stdout, "shm system is not initialized, init it.\n");
                initialized();
            }
            else
            {
                shm_mem_sys_control_->crc = rec_crc;
            }
        }

        {
            // Check if all processes in proc map have exited
            bool allProcessesExited = true;
            for (const auto& entry : *shm_mem_sys_control_->shm_proc_map_) {
                pid_t pid = entry.first;
                if (kill(pid, 0) == 0 || errno != ESRCH) {
                    allProcessesExited = false;
                    break;
                }
            }

            // If all processes have exited, reinitialize the system
            if (allProcessesExited) {
                fprintf(stdout, "All processes in proc map have exited. Reinitializing system.\n");
                initialized();
            }
        }

        // remember self pid
        shm_mem_sys_control_->shm_proc_map_->emplace(self_pid_, self_pid_);
    }
}

ShmObjManager::~ShmObjManager()
{
    shm_obj_map_.clear();

    FileLockGuard lock(key_file_lock_);

    shm_mem_sys_control_->shm_proc_map_->erase(self_pid_);

    // 检查挂载的进程是否仍然有效
    for(auto it = shm_mem_sys_control_->shm_proc_map_->begin(); it != shm_mem_sys_control_->shm_proc_map_->end(); it)
    {
        const auto proc_file = "/proc/" + std::to_string(it->first);

        if(access(proc_file.c_str(), F_OK) == 0)
        {
            it++;
        }
        else
        {
            it = shm_mem_sys_control_->shm_proc_map_->erase(it);
        }

    }
    // 如果 proc map 为空则销毁内存
    if(0 == shm_mem_sys_control_->shm_proc_map_->size())
    {
        __inner::ShmMemory::mem().destory();
        std::cout << "destory shm memory" << std::endl;
    }
}

ShmObjManager &ShmObjManager::instance()
{
    static ShmObjManager shm_obj_manager;
    return shm_obj_manager;
}

uint64_t ShmObjManager::getSharedObjMapUpdateTime()
{
    return shm_mem_sys_control_->getUpdateTime();
}

bool ShmObjManager::hasSharedObject(const std::string &name)
{
    {
        auto itor = shm_obj_map_.find(name);
        if(itor != shm_obj_map_.end())
        {
            return true;
        }
    }

    {
        ProcessLockGuard<ProcessMutex> lock(shm_mem_sys_control_->mutex);
        auto itor = shm_mem_sys_control_->shm_obj_map_->find(__inner::ShmObjString(name));
        if(itor != shm_mem_sys_control_->shm_obj_map_->end())
        {
            return true;
        }
    }

    return false;
}

void ShmObjManager::printObjName()
{
    // for(auto& m : shm_obj_map_)
    // {
    //     std::cout << ">>" << m.first << std::endl;
    // }
    // {
    //     ProcessLockGuard<ProcessMutex> lock(shm_mem_sys_control_->mutex);

    //     for(auto m = shm_mem_sys_control_->shm_obj_map_->begin();
    //         m != shm_mem_sys_control_->shm_obj_map_->end(); m++)
    //     {
    //         std::cout << "--" << m->first << std::endl;
    //     }
    // }
}

ShmMemSystemControl::ShmMemSystemControl()
{
    static_assert(sizeof(__inner::ShmObjMap<__inner::ShmObjString, ShmObjInfo>) + sizeof(std::atomic< uint64_t >) <= __inner::ShmMemory::PAGE_SIZE, "sizeof(ShmMemSystemControl) <= PAGE_SIZE");
    static_assert(sizeof(__inner::ShmObjMap<uint64_t, uint64_t>) <= __inner::ShmMemory::PAGE_SIZE, "sizeof(ShmMemSystemControl) <= PAGE_SIZE");

    __inner::ObjAllocator<__inner::ShmObjVector<pid_t>> alloc;

    this->magic_value = shm_system_magic_key;
    this->shm_obj_map_ = new (__inner::ShmMemory::mem().obj_map_addr())(ShmMemSystemObjMap);
    this->shm_proc_map_ = new (__inner::ShmMemory::mem().proc_map_addr())(ShmMemSystemProcMap);
    this->update_time_ = new ((uint8_t*)__inner::ShmMemory::mem().obj_map_addr() + sizeof(ShmMemSystemObjMap))(std::atomic< uint64_t >);
    alloc.construct(this->shm_proc_map_);
    this->crc = 0;
    this->crc = coin::calculate_crc32(0, (const unsigned char*)this, sizeof(ShmMemSystemControl));
}

ShmMemSystemControl::~ShmMemSystemControl()
{
}

} // namespace coin::data
