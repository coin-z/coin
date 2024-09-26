/**
 * @file communicator.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <regex>
#include <coin-commons/utils/strings.hpp>
#include <code_scan/grammar/communicator.hpp>

namespace coin::code
{
const std::string GrammarCommunicator::code("communicator");

GrammarCommunicator::GrammarCommunicator(const MateObject::Ptr mo)
{
    // 计算 path
    auto p = mo->parent();
    while(p)
    {
        path_ = std::string(p->name()) + "/" + path_;
        p = p->parent();
    }

    // 计算 comm type
    std::regex re("<(.+)>");
    std::match_results<std::string::const_iterator> matchs;
    if(std::regex_search(mo->key_word().begin(), mo->key_word().end(), matchs, re))
    {
        comm_type_ = matchs.str();
        auto no_head_end = std::remove(comm_type_.begin(), comm_type_.end(), '<');
        comm_type_.erase(no_head_end, comm_type_.end());
        auto no_tail_end = std::remove(comm_type_.begin(), comm_type_.end(), '>');
        comm_type_.erase(no_tail_end, comm_type_.end());
    }

    // 计算 data type
    data_type_ = coin::strings::remove_leading_chars(mo->name(), " ");

    // 计算 name
    std::regex name_re("(:|;)");
    std::match_results<std::string::const_iterator> name_matchs;
    if(std::regex_search(mo->depict().begin(), mo->depict().end(), name_matchs, name_re))
    {
        name_ = mo->depict().substr(0, name_matchs.position());
    }
    name_ = coin::strings::remove_leading_chars(name_, " ");
}

const std::string_view GrammarCommunicator::id() const
{
    return code;
}
const std::string GrammarCommunicator::regex()
{
    return ("(" + code + "|" + code + "<.+>)");
}
} // namespace coin::code
