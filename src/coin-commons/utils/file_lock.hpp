/**
 * @file file_lock.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once
#include <string>

namespace coin
{
class FileLock
{
public:
    FileLock(const std::string& path) noexcept;
    ~FileLock() noexcept;

    void lock() noexcept;
    void unlock() noexcept;

private:
    std::string file_path_;
    int file_descriptor_;
};

class FileLockGuard
{
public:
    FileLockGuard(FileLock& lock) noexcept;
    ~FileLockGuard() noexcept;

private:
    FileLock& lock_;
};
} // namespace coin
