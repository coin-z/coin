/**
 * @file frame.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include <code_scan/grammar/base.hpp>


namespace coin::code
{
class GrammarFrame : public GrammarBase
{
public:
    using Ptr = std::shared_ptr<GrammarFrame>;
    GrammarFrame(const MateObject::Ptr mo);
    virtual ~GrammarFrame() = default;

    static const std::string code;

    virtual inline const std::string_view path() const override { return path_; }
    virtual inline const std::string regex_word() const override { return regex(); }
    virtual const std::string_view id() const override { return code; };
    static const std::string regex();
    inline const std::string_view name() const { return name_; }

    virtual inline const CodeType code_type() const override { return CodeType::FRAME; }

private:
    std::string path_;
    std::string name_;
};
} // namespace coin::code