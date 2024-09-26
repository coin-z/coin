/**
 * @file service.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-26
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

    auto service = coin::data::Communicator::service<ReqTest, AckTest>("sercli", 
        [](coin::data::Service<ReqTest, AckTest>::ConstReqPtr& req, coin::data::Service<ReqTest, AckTest>::AckPtr& ack) -> bool{
            std::cout << "Req is: " << req->msg << std::endl;
            auto time = coin::getCurrentMsTime();
            ack->msg = "i am ok: " + std::to_string(time);
            return true;
        });

    while(coin::data::ok())
    {
        coin::data::spin_once();
        usleep(1);
    }

    sleep(1);

    return 0;
}

