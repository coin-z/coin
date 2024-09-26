/**
 * @file reader.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-25
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

    auto reader = coin::data::Communicator::reader<coin::data::ShmString>("rw");

    while(coin::data::ok())
    {
        if(reader->isUpdate())
        {
            coin::data::Reader<coin::data::ShmString>::DataPtr data = reader->read();
            std::cout << *data << std::endl;
        }

        coin::data::spin_once();
        usleep(10);
    }

    return 0;
}

