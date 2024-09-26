/**
 * @file code_tree_config.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include <string>
#include <vector>

namespace coin::code
{
class CodeTreeConfig
{
public:
    CodeTreeConfig();
    ~CodeTreeConfig() = default;
    const std::vector<std::string> default_search_dirs;
    const std::string suffix;

    std::vector<std::string> registed_search_dirs;
    
    const std::vector<std::string> all_search_dirs() const;
};
} // coin::code