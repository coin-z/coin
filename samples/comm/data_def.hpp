/**
 * @file data_def.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once
#include <local/local_channal.hpp>

class DataTest
{

public:
    DataTest(int a, int b) : a_(a), b_(b) {}

    ~DataTest()
    {
        // std::cout << "release data test: " << a_ << ", " << b_ << std::endl;
    }

    inline int a() const { return a_; }
    inline int b() const { return b_; }
    inline uint64_t ts() const { return ts_; }
    inline void setTime(const uint64_t ts) { ts_ = ts; }
    uint8_t data[1024 * 1024];

private:
    int a_ = 0;
    int b_ = 0;
    uint64_t ts_ = 0;

};

struct ReqTest
{
    coin::data::ShmString msg;

    inline ReqTest(const std::string& m) : msg(m) {}

    inline ~ReqTest()
    {
        std::cout << "req release." << std::endl;
    }
};

struct AckTest
{
    coin::data::ShmString msg;

    inline ~AckTest()
    {
        std::cout << "ack release." << std::endl;
    }
};
