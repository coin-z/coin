/**
 * @file code_tree.config.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <code_scan/config/code_tree.config.hpp>

#include <stdlib.h>

namespace coin::code
{
static std::vector< std::string > default_search_dirs_()
{
    std::vector<std::string> dirs;
    // 默认搜索当前路径
    dirs.push_back(std::string("."));
    // 计算系统路径
    char* env = getenv("COIN_ROOT");
    if(env)
    {
        dirs.push_back(std::string(env) + "/" + "data.co");
    }
    return std::move(dirs);
}

CodeTreeConfig::CodeTreeConfig()
  : default_search_dirs(default_search_dirs_())
  , suffix(".co")
{

}

const std::vector<std::string> CodeTreeConfig::all_search_dirs() const
{
    std::vector<std::string> dirs = default_search_dirs;
    dirs.insert(dirs.end(), registed_search_dirs.begin(), registed_search_dirs.end());
    return std::move(dirs);
}
} // namespace coin::code
