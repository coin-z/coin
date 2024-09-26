/**
 * @file code_tree.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "code_tree.hpp"
#include <filesystem>
#include <set>
#include <code_backend/cpp_backend.hpp>
#include <coin-commons/utils/utils.hpp>

namespace coin::code
{
CodeException::CodeException(const std::string& what) : what_(what) {}

CodeTree::CodeTree()
  : code_root_(make_ptr<CodeNode>())
  , config_(make_ptr<CodeTreeConfig>())
{
}
std::map<std::string, CodeTree::ptr_t<const CodeDecoder>> CodeTree::load_code_file_list(const std::vector<std::string> &file_list)
{
    std::map<std::string, CodeTree::ptr_t<const CodeDecoder>> decoder_list;
    for(auto& name : file_list)
    {
        std::filesystem::path file(name);
        // 检查文件是否存在，如果不存在则跳过
        if(not std::filesystem::exists(file))
        {
            std::stringstream ss;
            ss << "file <" << name << "> not exists";
            Print::error(ss.str());
            throw CodeException(ss.str());
        }

        // 计算绝对路径
        std::filesystem::path abs_path = std::filesystem::weakly_canonical(file);

        ptr_t<CodeDecoder> decoder = find_decoder(code_root_, abs_path.string());
        // 如果没有找打则进行处理，如过已经找到则直接使用
        if(not decoder)
        {
            Print::info("{} load decoder from file: <{}>", "🚀", abs_path.string());

            auto search_path = calculate_search_path(*config_, abs_path.string());

            // 创建 code node
            decoder = make_ptr<CodeDecoder>();
            std::map<std::string, ptr_t<const CodeDecoder>> import_decoder_list;
            decoder->load_source_file(file);
            decoder->update_search_path(search_path);
            decoder->remove_comments();
            decoder->scan_objects();

            // 提取依赖列表
            {
                std::vector<std::string> import_file_list;
                CodeTypeTree type_tree;
                read_and_check_import_list(config_, decoder, import_file_list, type_tree);
                // 如果有新的依赖，则添加
                if(not import_file_list.empty())
                {
                    import_decoder_list = load_code_file_list(import_file_list);
                    decoder->set_import_list(import_decoder_list);
                }
            };
            // 检查合法性
            {
                auto ret = decoder->check_is_valid(config_, import_decoder_list);
                if(not ret)
                {
                    std::stringstream ss;
                    ss << "invalid code: " << decoder->root() << ", " << ret.reason();
                    Print::error(ss.str());
                    throw CodeException(ss.str());
                }
            }

            // 记录构建文件清单
            file_list_.emplace_back(abs_path.string());

            // 加入构建树
            add_code2tree(code_root_, "/", abs_path.parent_path().string().substr(1), decoder);
            Print::info("{} decoder <{}> loaded", "😁", abs_path.string());
        }
        decoder_list.emplace(std::filesystem::weakly_canonical(file).string(), decoder);
    }
    return std::move(decoder_list);
}
void CodeTree::generate_target(const std::vector<std::string>& file_list, const std::string& output)
{
    Print::tab_debuger gen_debuger;
    Print::info("🚀 process code tree");
    static auto process_path = [](const std::string& path) {
        if(path[0] != '/')
        {
            return std::filesystem::weakly_canonical("./" + path);
        }
        return std::filesystem::weakly_canonical(path);
    };

    auto gen2file = [this](Print::tab_debuger& gen_debuger, ptr_t<CodeDecoder>& decoder, const std::string& out_path) -> std::string {
        Print::tab_debuger::debuger_guard debuger(gen_debuger);
        auto abs_search_path = std::filesystem::weakly_canonical(decoder->search_path());
        auto search_path_hash = std::to_string( std::hash<std::string>{}(abs_search_path.string()) );
        auto abs_source = std::filesystem::weakly_canonical(decoder->path());
        auto relate_path = std::filesystem::relative(decoder->path(), abs_search_path);
        auto abs_out_path = std::filesystem::weakly_canonical(out_path) / std::filesystem::path(search_path_hash) / relate_path;
        // 计算相对路径
        gen_debuger.print("search: {}", abs_search_path.string());
        gen_debuger.print("source: {}", abs_source.string());
        gen_debuger.print("output: {}", abs_out_path.string());

        // 查找 decoder
        if(not decoder)
        {
            std::stringstream ss;
            ss << "decoder not found: " << abs_source.string();
            gen_debuger.no("{}", ss.str());
            throw CodeException(ss.str());
        }
        // 根据 decoder 生成 code
        ptr_t<CppBackend> cpp_backend = make_ptr<CppBackend>(decoder, abs_out_path.string());
        cpp_backend->generate();

        return search_path_hash;
    };

    gen_debuger.print("start process input files.");
    {
        Print::tab_debuger::debuger_guard debuger(gen_debuger);

        std::set<std::string> output_hash_list;

        foreach_decoder(code_root_, [&output_hash_list, &gen2file, &gen_debuger, &output](const std::string& path, ptr_t<CodeDecoder> decoder) {
            Print::info("🚀 generate target of <{}>", decoder->path());
            auto hash = gen2file(gen_debuger, decoder, process_path(output));
            if(output_hash_list.find(hash) == output_hash_list.end())
            {
                output_hash_list.emplace(hash);
            }
            Print::info("😁 <{}> done.", decoder->path());
        });

        if(not target_.empty())
        {
            CppBackend::generate_cmake(output_hash_list, output, target_);
        }
    }
    Print::info("😁 process code tree done");
}
void CodeTree::regist_search_path(const std::string& path)
{
    if(config_)
    {
        config_->registed_search_dirs.push_back(path);
    }
}
void CodeTree::regist_search_path(const std::vector<std::string>& paths)
{
    if(config_)
    {
        config_->registed_search_dirs.insert(config_->registed_search_dirs.end(), paths.begin(), paths.end());
    }
}
void CodeTree::set_target(const std::string& t)
{
    target_ = t;
}
std::string CodeTree::calculate_valid_path(const std::string &path, const std::string& file, const std::string& file_suffix)
{
    // 使用 path 和 file 拼接完整路径，并逐步回退file的路径，直到找到一个合法文件
    std::string_view check_file = file;
    auto pos = check_file.size();
    auto result = std::filesystem::weakly_canonical(path) / (file + file_suffix);
    while(true)
    {
        // 如果此文件存在则返回
        if(std::filesystem::exists(result))
        {
            // 必须是一个文件
            if(std::filesystem::is_regular_file(result))
            {
                return std::filesystem::weakly_canonical(result).string();
            }
            else
            {
                return std::string();
            }
        }
        // 不存在则回退一层
        pos = check_file.find_last_of("/");
        if(pos == std::string::npos) break;
        check_file.remove_suffix(check_file.size() - pos);
        result = std::filesystem::weakly_canonical(path) / (std::string(check_file) + file_suffix);
    }
    return std::string();
}
void CodeTree::read_and_check_import_list(const ptr_t<CodeTreeConfig>& config, 
                                           ptr_t<CodeDecoder>& decoder, 
                                           std::vector<std::string>& import_file_list,
                                           CodeTypeTree& type_tree)
{
    auto import_list = decoder->import_package_list();
    for(auto& import : import_list)
    {
        auto import_list_info = CodeDecoder::read_import_path_and_package(config, import);
        for(auto& info : import_list_info)
        {
            if(not info.first.empty())
            {
                import_file_list.push_back(info.first);
            }
            else
            {
                std::stringstream ss;
                ss << "invalid import path: " << info.first << ", " << info.second << ", import: " << import;
                Print::error(ss.str());
                throw CodeException(ss.str());
            }
        }
    }
}
void CodeTree::add_code2tree(ptr_t<CodeNode>& node, const std::string& parent, const std::string& cpath, ptr_t<CodeDecoder>& code)
{
    // cpath 遍历到最后一层，将 code 加入
    if(cpath.empty())
    {
        if(node)
        {
            auto itor = node->codes.find(code->root());
            // 如果不存在则添加，已存在则放弃
            if(itor == node->codes.end())
            {
                node->codes.emplace(code->root(), code);
            }
            else
            {
                Print::debug("already exist code: %s", code->root().c_str());
            }
        }
        return;
    }

    //  检查是否存在同名的下级路径
    auto pos = cpath.find_first_of("/");
    auto p = cpath.substr(0, pos);
    std::string last_cpath = "";
    if(pos != std::string::npos)
    {
        last_cpath = cpath.substr(pos + 1);
    }

    ptr_t<CodeNode> next_node = nullptr;
    auto next_node_itor = node->next.find(p);
    if(next_node_itor == node->next.end())
    {
        next_node = make_ptr<CodeNode>();
        next_node->previous = node; // 记录父路径
        node->next.emplace(p, next_node);
    }
    else
    {
        // 已经存在，则直接获取
        next_node = next_node_itor->second;
    }
    // 继续遍历
    std::string cparent = parent + p + "/";
    add_code2tree(next_node, cparent, last_cpath, code);
}

CodeTree::ptr_t<CodeDecoder> CodeTree::find_decoder(ptr_t<CodeNode>& node, const std::string &path)
{
    std::string_view path_view = path;
    if(path[0] == '/')
    {
        path_view.remove_prefix(1);
    }
    auto pos = path_view.find_first_of("/");
    auto p = std::string( path_view.substr(0, pos) );
    auto itor = node->next.find(p);
    if(itor != node->next.end())
    {
        return find_decoder(itor->second, path.substr(pos + 1));
    }
    else
    {
        auto code_itor = node->codes.find(p);
        if(code_itor != node->codes.end())
        {
            return code_itor->second;
        }
    }
    return nullptr;
}
bool CodeTree::is_load2tree(ptr_t<CodeNode>& node, const std::string &path)
{
    return find_decoder(node, path) != nullptr;
}
void CodeTree::foreach_decoder(ptr_t<CodeNode>& node, const std::function<void(const std::string&, ptr_t<CodeDecoder>)>& func)
{
    for(auto& code : node->codes)
    {
        func(code.first, code.second);
    }
    for(auto& next : node->next)
    {
        foreach_decoder(next.second, func);
    }
}
std::string CodeTree::calculate_search_path(const CodeTreeConfig& config, const std::string& file)
{
    static auto is_subpath = [](const std::string& path, const std::string& target) {
        auto abs_path = std::filesystem::weakly_canonical(path);
        auto abs_target = std::filesystem::weakly_canonical(target);
        auto path_itor = abs_path.begin();
        auto target_itor = abs_target.begin();
        while(path_itor != abs_path.end() and target_itor != abs_target.end())
        {
            if(*path_itor != *target_itor)
            {
                return false;
            }
            path_itor++;
            target_itor++;
        }
        return (path_itor == abs_path.end());
    };

    for(auto& path : config.all_search_dirs())
    {
        if(is_subpath(path, file))
        {
            return path;
        }
    }

    return "";
}
} // namespace coin::code
