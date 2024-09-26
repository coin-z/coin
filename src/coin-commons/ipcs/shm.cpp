/**
 * @file shm.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "ipcs/shm.hpp"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>

namespace coin::ipc
{
/**
 * @brief implementation of Shm by POSIX standard API
 * 
 */
Shm::Shm(const std::string& key_file, void* addr, const size_t size)
  : key_file_(key_file)
  , key_(get_key_(key_file))
  , req_addr_(addr)
  , addr_(nullptr)
  , size_(size)
{ }
Shm::~Shm()
{ }
key_t Shm::get_key_(const std::string& key_file)
{
    key_t key = -1;
    if(0 == access(key_file.c_str(), F_OK))
    {
        // calculate key
        key = ftok(key_file.c_str(), 0);
    }
    return key;
}
int Shm::num_of_attach()
{
    struct shmid_ds shmds;
    memset(&shmds, 0, sizeof(shmds));
    key_t key = get_key_(key_file_);
    if(key > 0)
    {
        shmctl(shmget(key, 0, 0), IPC_STAT, &shmds);
    }
    return shmds.shm_nattch;
}
bool Shm::is_finnally()
{
    return (num_of_attach() == 1);
}
int Shm::create()
{
    if(key_ < 0) return -1;
    auto id = shmget(key_, size_, IPC_CREAT | IPC_EXCL | 0600);
    if(id < 0) return -1;
    return 0;
}
int Shm::destroy()
{
    if(key_ < 0) return -1;
    auto id = shmget(key_, 0, 0);
    if(id < 0) return -1;
    shmctl(id, IPC_RMID, NULL);
    return 0;
}
int Shm::attach()
{
    if(key_ < 0) return -1;
    auto id = shmget(key_, 0, 0);
    if(id < 0) return -1;
    addr_ = shmat(id, req_addr_, 0);
    return 0;
}
int Shm::detach()
{
    int ret = 0;
    if(addr_)
    {
        ret = shmdt(addr_);
        addr_ = nullptr;
    }
    return ret;
}
void* Shm::addr()
{
    return addr_;
}
size_t Shm::size()
{
    return size_;
}
} // namespace coin::ipc
