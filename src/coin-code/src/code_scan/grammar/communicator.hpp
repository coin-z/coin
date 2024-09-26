/**
 * @file communicator.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once
#include <string>
#include <string_view>

#include <code_scan/grammar/base.hpp>

namespace coin::code
{
class GrammarCommunicator : public GrammarBase
{
public:
    using Ptr = std::shared_ptr<GrammarCommunicator>;
    GrammarCommunicator(const MateObject::Ptr mo);
    virtual ~GrammarCommunicator() = default;

    static const std::string code;

    virtual const std::string_view id() const override;
    virtual inline const std::string_view path() const override { return path_; }
    virtual inline const std::string regex_word() const override { return regex(); }
    static const std::string regex();

    inline const std::string& comm_type() const { return comm_type_; }
    inline const std::string& data_type() const { return data_type_; }
    inline const std::string& name() const { return name_; }

    virtual inline const CodeType code_type() const override { return CodeType::COMMUNICATOR; }

private:
    std::string comm_type_;
    std::string data_type_;
    std::string name_;
    std::string path_;
};
} // namespace coin::code
