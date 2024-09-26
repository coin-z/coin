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
#include <local/local_channal.hpp>

#include "data_def.hpp"



int main(int argc, char *argv[])
{
    constexpr int idx = 10;
    int cnt = idx;

    coin::data::init(argc, argv);

    auto sub_callback = [](coin::data::local::LocalSubscriber<DataTest>::ConstDataPtr data) {
        std::cout << ", data: " << data->a() << ", " << data->b() << ", " << std::string((char*)data->data) << std::endl;
    };

    auto sub = coin::data::local::LocalChannal::subscriber<DataTest>("pub", sub_callback);

    sleep(1);

    // sub.reset();

    // sub = coin::data::local::LocalChannal::subscriber<DataTest>("pub", sub_callback);

    uint64_t i = 0;
    while(coin::data::ok())
    {
        if(i++ > 100000)
        {
            i = 0;
            sub.reset();
            std::cout << "reset pub" << std::endl;
            sub = coin::data::local::LocalChannal::subscriber<DataTest>("pub", sub_callback);
        }
        coin::data::local::LocalChannal::instance().spin_once();
        usleep(10);
    }
    // sleep(100);

    return 0;
}

