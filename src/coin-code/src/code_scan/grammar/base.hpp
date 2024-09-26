/**
 * @file base.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include <memory>

#include <code_scan/grammar/mate_object.hpp>

namespace coin::code
{
class GrammarBase
{
public:
    using Ptr = std::shared_ptr<GrammarBase>;
    enum class CodeType {
        UNKNOWN = 0,
        FRAME = 1,
        FIELD = 2,
        COMMUNICATOR = 3,
    };
    GrammarBase() = default;
    virtual ~GrammarBase() = default;

    virtual const std::string_view id() const = 0;
    virtual const std::string_view path() const = 0;
    virtual const std::string regex_word() const = 0;

    virtual inline const CodeType code_type() const { return CodeType::UNKNOWN; }
};
} // namespace coin::code