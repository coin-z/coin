/**
 * @file code_tree.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include <code_scan/code_decoder.hpp>
#include <code_scan/config/code_tree.config.hpp>
#include <code_scan/config/code_exception.hpp>
#include <code_scan/type_tree/type_tree.hpp>

#include <string>
#include <map>

namespace coin::code
{
class CodeTree
{
    template <typename T>
    using ptr_t = std::shared_ptr<T>;
    template <typename T, typename... ArgsT>
    inline static ptr_t<T> make_ptr(ArgsT&&... args) { return std::make_shared<T>(std::forward<ArgsT>(args)...); }

public:
    CodeTree();
    ~CodeTree() = default;

    std::map<std::string, ptr_t<const CodeDecoder>> load_code_file_list(const std::vector<std::string>& file_list);
    void generate_target(const std::vector<std::string>& file_list, const std::string& output);
    void regist_search_path(const std::string& path);
    void regist_search_path(const std::vector<std::string>& paths);
    void set_target(const std::string& t);

private:
    struct CodeNode
    {
        ptr_t<CodeNode> previous;
        std::map<std::string, ptr_t<CodeNode>> next;
        std::map<std::string, ptr_t<CodeDecoder>> codes;
    };
    ptr_t<CodeNode> code_root_;
    std::vector<std::string> file_list_;
    ptr_t<CodeTreeConfig> config_;
    std::string target_;

private:
    static void read_and_check_import_list(const ptr_t<CodeTreeConfig>& config, 
                                            ptr_t<CodeDecoder>& decoder,
                                            std::vector<std::string>& import_file_list,
                                            CodeTypeTree& type_tree);
    static void add_code2tree(ptr_t<CodeNode>& node, const std::string& parent, const std::string& cpath, ptr_t<CodeDecoder>& code);
    static bool is_load2tree(ptr_t<CodeNode>& node, const std::string& path);
    static ptr_t<CodeDecoder> find_decoder(ptr_t<CodeNode>& node, const std::string& path);
    static void foreach_decoder(ptr_t<CodeNode>& node, const std::function<void(const std::string&, ptr_t<CodeDecoder>)>& func);
    static std::string calculate_valid_path(const std::string& path, const std::string& file, const std::string& file_suffix = "");

    static std::string calculate_search_path(const CodeTreeConfig& config, const std::string& file);
};
} // namespace coin::code
