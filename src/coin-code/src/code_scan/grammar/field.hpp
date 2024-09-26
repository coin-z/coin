/**
 * @file field.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include <code_scan/grammar/base.hpp>

namespace coin::code
{
class GrammarField : public GrammarBase
{
public:
    using Ptr = std::shared_ptr<GrammarField>;
    GrammarField(const MateObject::Ptr mo);
    virtual ~GrammarField() = default;

    static const std::string code;

    virtual const std::string_view id() const override;
    virtual inline const std::string_view path() const override { return path_; }
    virtual inline const std::string regex_word() const override { return regex(); }
    static const std::string regex();

    inline const std::string_view type() const { return type_; }
    inline const std::string_view name() const { return name_; }
    inline const std::string_view idx() const { return idx_; }
    inline const std::string_view default_value() const { return default_value_; }
    inline const std::string_view range() const { return range_; }
    
    virtual inline const CodeType code_type() const override { return CodeType::FIELD; }

private:

    // int b: 2 =10 @[];
    // string m: 3 ="hello;";
    std::string path_;
    std::string type_;
    std::string name_;
    std::string idx_;
    std::string default_value_;
    std::string range_;
};
} // namespace coin::code
