/**
 * @file crc32.h
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include <stdint.h>

namespace coin
{
uint32_t calculate_crc32(uint32_t seed, const unsigned char *buf, size_t len);
}
