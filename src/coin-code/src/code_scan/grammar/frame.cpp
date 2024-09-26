/**
 * @file frame.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <code_scan/grammar/frame.hpp>
#include <coin-commons/utils/utils.hpp>
#include <coin-commons/utils/strings.hpp>
#include "frame.hpp"

namespace coin::code
{
const std::string GrammarFrame::code = "frame";
GrammarFrame::GrammarFrame(const MateObject::Ptr mo)
{
    // 计算 path
    auto p = mo->parent();
    while(p)
    {
        path_ = std::string(p->name()) + "/" + path_;
        p = p->parent();
    }
    auto pos = mo->code_block().find_first_of(' ');
    name_ = mo->code_block().substr(pos);
    name_ = coin::strings::remove_leading_chars(path_, " ");
    name_ = name_.substr(0, name_.find_first_of(' '));
}
const std::string GrammarFrame::regex()
{
    return ("(" + code + "|" + code + "<.+>)");
}
} // namespace coin::code

