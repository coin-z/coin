/**
 * @file writer.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <coin-data.hpp>
#include <local/local_channal.hpp>

#include "data_def.hpp"



int main(int argc, char *argv[])
{
    coin::data::init(argc, argv);

    auto writer = coin::data::local::LocalChannal::writer<coin::data::ShmString>("writer");
    int cnt = 0;
    while(coin::data::ok())
    {
        std::string msg = "hello, i count to " + std::to_string(cnt++);
        auto data = coin::data::makeShmShared<coin::data::ShmString>(msg);
        writer->write(data);
        coin::data::local::LocalChannal::instance().spin_once();
        usleep(100000);
    }

    return 0;
}

