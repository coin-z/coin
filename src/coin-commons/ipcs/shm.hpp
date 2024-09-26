/**
 * @file shm.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include <string>

namespace coin::ipc
{
class Shm
{
public:
    Shm(const std::string& key_file, void* addr, const size_t size);
    ~Shm();

    int num_of_attach();
    int create();
    int destroy();
    int attach();
    int detach();
    bool is_finnally();

    void* addr();
    size_t size();

private:
    key_t get_key_(const std::string& key_file);

private:
    const std::string key_file_;
    const key_t key_;
    const void* req_addr_;
    void* addr_;
    const size_t size_;
}; // class Shm
} // namespace coin
