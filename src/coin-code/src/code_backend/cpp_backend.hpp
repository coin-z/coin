/**
 * @file cpp_backend.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include <memory>
#include <map>
#include <set>
#include <code_scan/code_decoder.hpp>

namespace coin::code
{
class CppBackend
{
public:
    explicit CppBackend(const std::shared_ptr<CodeDecoder>& decoder, const std::string& output);
    ~CppBackend() = default;

    void generate();
    static void generate_cmake(const std::set<std::string>& inc_dirs, const std::string& output, const std::string& target);

private:
    std::shared_ptr<CodeDecoder> decoder_;
    std::string output_path_;
    std::string output_cpp_;
    std::string output_hpp_;

    const std::map<std::string, std::function<std::string(const std::shared_ptr<const GrammarField>& g, const std::string& code)>> type_map_;
    const std::map<std::string, std::function<std::string(const std::string&)>> range_map_;
    std::string gen_type_(const std::shared_ptr<const CodeDecoder>& decoder, const std::shared_ptr<const GrammarField>& g, const std::string& code_block);
    std::string gen_type_(const std::shared_ptr<const CodeDecoder>& decoder, const std::shared_ptr<const GrammarCommunicator>& g, const std::string& code_block);
    std::string get_type_name_(const std::shared_ptr<const CodeDecoder>& decoder, const std::shared_ptr<const GrammarField>& g, const std::string& code_block);
    std::string cvt_communicator_type(const std::shared_ptr<const CodeDecoder>& decoder, const std::shared_ptr<const GrammarCommunicator>& t, const std::string& code_block);

    std::string cvt_enum_type_(const std::shared_ptr<const GrammarField>& g, const std::string& code);

}; // class CppBackend
} // namespace coin::code