/**
 * @file client.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <coin-data.hpp>
#include <local/local_channal.hpp>
#include <sys/types.h>
#include <sys/wait.h>
#include "data_def.hpp"


int main(int argc, char *argv[])
{
    coin::data::init(argc, argv);

    constexpr int idx = 10;
    int cnt = idx;

    auto client = coin::data::local::LocalChannal::client<ReqTest, AckTest>("service");

    sleep(1);

    while(coin::data::ok())
    {
        std::cout << "----------------------------------------------" << std::endl;
        std::string msg = "hello, i count to " + std::to_string(cnt++);
        coin::data::local::LocalClient<ReqTest, AckTest>::ReqPtr req = coin::data::makeShmShared<ReqTest>(msg);
        coin::data::local::LocalClient<ReqTest, AckTest>::AckPtr ack = coin::data::makeShmShared<AckTest>();
        if(client->call(req, ack))
        {
            std::cout << "ack: " << ack->msg << std::endl;
        }
        coin::data::local::LocalChannal::instance().spin_once();
        sleep(1);
    }

    return 0;
}

