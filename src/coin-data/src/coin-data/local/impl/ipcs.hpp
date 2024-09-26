/**
 * @file ipcs.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __IPCS_H__
#define __IPCS_H__

#include <stdint.h>
#include <stddef.h>
#include <memory>
#include <coin-commons/ipcs/shm.hpp>


namespace coin::data
{
class Shm
{
public:
    Shm(const std::string& key, void* addr, size_t size);
    ~Shm();

    void* addr();
    size_t size();

private:
    std::shared_ptr<coin::ipc::Shm> shm_;
}; // class Shm
}
#endif // __IPCS_H__
