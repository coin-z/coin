/**
 * @file field.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <code_scan/grammar/field.hpp>
#include <coin-commons/utils/strings.hpp>
#include <coin-commons/utils/utils.hpp>

#include <regex>
#include <iostream>

namespace coin::code
{
const std::string GrammarField::code = "field";
GrammarField::GrammarField(const MateObject::Ptr mo)
{
    // 计算 path
    auto p = mo->parent();
    while(p)
    {
        path_ = std::string(p->name()) + "/" + path_;
        p = p->parent();
    }
    // 提取 type
    type_ = mo->key_word();
    // 提取 name
    {
        name_ = mo->name();
        // 移除尾部的冒号
        auto pos = name_.find_last_of(":");
        name_ = name_.substr(0, pos);
    }
    std::string code_block = mo->code_block();
    // 移除尾部的 ;
    auto ritor = std::find_if(code_block.rbegin(), code_block.rend(), [](const char c){
        if(c != ' ' && c != ';')
        {
            return true;
        }
        return false;
    });
    if(ritor != code_block.rend())
    {
        code_block.erase(code_block.rend() - ritor);
    }
    // 提取 idx
    try {
        /*
        匹配 : 后面的数字
        移除 : 的版本
        (?<=:)\s*\d+
        不移除 : 的版本，可使用 std::regex
        (:\s*\d+)
        */
        std::regex re(R"(:\s*\d+)");
        std::match_results<std::string::iterator> matchs;
        if(std::regex_search(code_block.begin(), code_block.end(), matchs, re))
        {
            idx_ = coin::strings::remove_leading_chars(matchs.str(), " :");
        }
    }
    catch(const std::regex_error& e)
    {
        std::cerr << "idx: " << code_block << ": " << e.what() << '\n';
    }
    // 提取 default value
    try {
        /*
        匹配等号后面的内容，忽略双引号、单引号、方括号和花括号内的内容，直到遇到第一个空格结束
        移除 = 的版本，但 std::regex 不支持断言
        (?<==)\s*(?:"[^"]*"|'[^']*'|\[[^\]]*\]|\{[^\}]*\}|[^'"\[\]\{\}\s]+)
        不移除 = 的版本，可使用 std::regex
        (=\s*(?:"[^"]*"|'[^']*'|\[[^\]]*\]|\{[^\}]*\}|[^'"\[\]\{\}\s]+))
        */
        std::regex re(R"(=\s*(?:"[^"]*"|'[^']*'|\[[^\]]*\]|\{[^\}]*\}|[^'"\[\]\{\}\s]+))");
        std::match_results<std::string::iterator> matchs;
        if(std::regex_search(code_block.begin(), code_block.end(), matchs, re))
        {
            default_value_ = coin::strings::remove_leading_chars(matchs.str(), " =");
        }
    }
    catch(const std::regex_error& e)
    {
        std::cerr << "default value: " << code_block << ": " << e.what() << '\n';
    }
    // 提取 range
    try {
        /*
        (@\s*(?:"[^"]*"|'[^']*'|\[[^\]]*\]|\{[^\}]*\}|[^'"\[\]\{\}\s]+))
        */
       std::regex re(R"(@\s*(?:"[^"]*"|'[^']*'|\[[^\]]*\]|\{[^\}]*\}|[^'"\[\]\{\}\s]+))");
       std::match_results<std::string::iterator> matchs;
       if(std::regex_search(code_block.begin(), code_block.end(), matchs, re))
       {
           range_ = coin::strings::remove_leading_chars(matchs.str(), " @");
       }
    }
    catch(const std::regex_error& e)
    {
        std::cerr << "range: " << code_block << ": " << e.what() << '\n';
    }
}

const std::string_view GrammarField::id() const
{
    return code;
}

const std::string GrammarField::regex()
{
    /*
    \b.+\s*((?:(?!["\']).)*?(?:\'[^\']*\'|\"[^\"]*\")*?)*?;
    */
    // return "\\b.+:.+\\s*((?:(?![\"\\\']).)*?(?:\\\'[^\\\']*\\\'|\\\"[^\\\"]*\\\")*?)*?;";
    return R"(^\b(?!communicator\b).+:.+\s*((?:(?!["\']).)*?(?:\'[^\']*\'|\"[^\"]*\")*?)*?;)";
}

} // namespace coin::code
