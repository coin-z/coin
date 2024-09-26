/**
 * @file allocator.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "allocator.hpp"


namespace coin::data::__inner
{

// 基础数据的定义
template class Allocator<int>;
template class Allocator<long>;
template class Allocator<long long>;
template class Allocator<unsigned int>;
template class Allocator<unsigned long>;
template class Allocator<double>;
template class Allocator<float>;

} // coin::data::__inner
