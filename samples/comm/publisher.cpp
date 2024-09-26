/**
 * @file publisher.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <coin-data.hpp>
#include <communicator.hpp>

#include "data_def.hpp"

int main(int argc, char *argv[])
{
    coin::data::init(argc, argv);
    constexpr int idx = 10;
    int cnt = idx;

    auto pub = coin::data::Communicator::publisher<DataTest>("pubsub");
    while(coin::data::ok())
    {
        auto data = coin::data::makeShmShared<DataTest>(cnt, cnt * 3);
        auto len = sprintf((char*)data->data, "a is: %d, b is: %d", data->a(), data->b());
        data->setTime(coin::getCurrentUsTime());
        pub->publish(data);
        cnt += 1;
        usleep(10000);
        // sleep(1);
    }

    sleep(1);

    return 0;
}

