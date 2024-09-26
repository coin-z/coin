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
#include <local/local_channal.hpp>

#include "data_def.hpp"


int main(int argc, char *argv[])
{
    constexpr int idx = 10;
    int cnt = idx;

    coin::data::init(argc, argv);

    auto service = coin::data::local::LocalChannal::service<ReqTest, AckTest>("service", 
        [](coin::data::local::LocalService<ReqTest, AckTest>::ConstReqPtr& req, coin::data::local::LocalService<ReqTest, AckTest>::AckPtr& ack) -> bool{
            std::cout << "Req is: " << req->msg << std::endl;
            auto time = coin::getCurrentMsTime();
            ack->msg = "i am ok: " + std::to_string(time);
            return true;
        });
    while(coin::data::ok())
    {
        coin::data::local::LocalChannal::instance().spin_once();
        usleep(1);
    }

    sleep(1);

    return 0;
}

