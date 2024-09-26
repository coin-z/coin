/**
 * @file code_exception.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include <string>
#include <exception>

namespace coin::code
{
class CodeException : public std::exception
{
public:
    CodeException(const std::string& what);
    ~CodeException() = default;
    const char* what() const noexcept
    {
        return what_.c_str();
    }
private:
    std::string what_;
};
} // namespace coin::code
