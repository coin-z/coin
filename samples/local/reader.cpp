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
#include <local/local_channal.hpp>

#include "data_def.hpp"


int main(int argc, char *argv[])
{
    coin::data::init(argc, argv);

    auto reader = coin::data::local::LocalChannal::reader<coin::data::ShmString>("writer");
    while(coin::data::ok())
    {
        if(reader->isUpdate())
        {
            coin::data::local::LocalReader<coin::data::ShmString>::DataPtr data = reader->read();
            std::cout << *data << std::endl;
        }

        coin::data::local::LocalChannal::instance().spin_once();
        usleep(10);
    }

    return 0;
}
