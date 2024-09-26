/**
 * @file coin-data.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <coin-data.hpp>
#include <communicator.hpp>
#include <stdio.h>
#include <coin-commons/utils/utils.hpp>

namespace coin::data::__inner
{
void init(int argc, char *argv[])
{
    coin::data::Communicator::init(argc, argv);
}
void spin_once()
{
    coin::data::Communicator::spin_once();
}
} // namespace coin::data

extern "C" {
__attribute__((constructor)) void coin_data_initialize_()
{
    std::cout << "coin data initialize" << std::endl;
    coin::__inner::register_init(&coin::data::__inner::init);
    coin::__inner::register_spin_once(&coin::data::__inner::spin_once);
}
}
