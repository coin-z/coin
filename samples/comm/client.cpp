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
#include <communicator.hpp>
#include "data_def.hpp"


int main(int argc, char *argv[])
{
    coin::data::init(argc, argv);

    constexpr int idx = 10;
    int cnt = idx;

    auto client = coin::data::Communicator::client<ReqTest, AckTest>("sercli");

    auto mut_client = coin::data::Communicator::client<coin::data::Communicator::MutableServerReq, coin::data::Communicator::MutableServerAck>(
        "$$/qwe/service/mutable_communicator"
    );

    auto req = coin::data::makeShmShared<coin::data::Communicator::MutableServerReq>();
    auto ack = coin::data::makeShmShared<coin::data::Communicator::MutableServerAck>();
    req->comm_id = "id";
    req->target_name = "name";
    auto ret = mut_client->call(req, ack);
    if(not ret)
    {
        std::cout << "call failed." << std::endl;
    }
    else
    {
        std::cout << "call done: " << ack->is_ok << ", " << ack->msg << std::endl;
    }

    std::cout << "run start..." << std::endl;

    while(coin::data::ok())
    {
        std::cout << "----------------------------------------------" << std::endl;
        auto ret = mut_client->call(req, ack);
        if(not ret)
        {
            std::cout << "call failed." << std::endl;
        }
        else
        {
            std::cout << "call done: " << ack->is_ok << ", " << ack->msg << std::endl;
        }
        std::string msg = "hello, i count to " + std::to_string(cnt++);
        coin::data::Client<ReqTest, AckTest>::ReqPtr req = coin::data::makeShmShared<ReqTest>(msg);
        coin::data::Client<ReqTest, AckTest>::AckPtr ack = coin::data::makeShmShared<AckTest>();
        if(client->call(req, ack))
        {
            std::cout << "ack: " << ack->msg << std::endl;
        }
        coin::data::spin_once();
        sleep(1);
    }
    return 0;
}

