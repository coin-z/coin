/**
 * @file subscriber.cpp
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
    constexpr int idx = 10;
    int cnt = idx;

    coin::data::init(argc, argv);

    auto pub = coin::data::Communicator::subscriber<DataTest>("pubsub", [](coin::data::Subscriber<DataTest>::ConstDataPtr data){
        // std::cout << "dt: " << std::dec << coin::data::utils::getCurrentUsTime() - data->ts() << ", data: " << data->a() << ", " << data->b() << ", " << std::string((char*)data->data) << std::endl;
    });

    uint64_t i = 100000;
    while(coin::data::ok())
    {
        coin::data::spin_once();
        usleep(10);
    }
    // sleep(100);

    return 0;
}

