#include <regex>
#include <iostream>
#include <fstream>
#include <deque>

#include <code_scan/code_decoder.hpp>
#include <code_scan/grammar/base.hpp>
#include <coin-commons/utils/strings.hpp>
#include <coin-commons/utils/utils.hpp>
#include "code_decoder.hpp"

namespace coin::code
{
template<typename T>
static GrammarBase::Ptr keywWordCreate(const MateObject::Ptr mo)
{ return std::make_shared<T>(mo); }

const std::map< std::string, std::function< GrammarBase::Ptr(const MateObject::Ptr) > > CodeDecoder::key_word_factory_map {
    {"frame", keywWordCreate<GrammarFrame>},
    {"field", keywWordCreate<GrammarField>},
    {"communicator", keywWordCreate<GrammarCommunicator>}
};

CodeDecoder::CodeDecoder() : object_(std::make_shared<MateObject>())
  , type_tree_(std::make_shared<CodeTypeTree>())
{
}

CodeDecoder::~CodeDecoder()
{
}

RetBool CodeDecoder::load_source_file(const std::filesystem::path &path)
{
    // Check if file exists
    if (!std::filesystem::exists(path))
    {
        path_.clear();
        return RetBool(false, "File does not exist: " + path.string());
    }

    path_ = path;

    std::ifstream file(path_);
    if(not file.is_open())
    {
        return RetBool(false, "Could not open file: " + path_.string());
    }

    // 获取文件名称
    root_ = path_.filename().string();

    source_text_stream_.clear();
    source_text_stream_ << file.rdbuf();
    code_text_ = source_text_stream_.str();

    return RetBool(true);
}
RetBool CodeDecoder::update_search_path(const std::filesystem::path& path)
{
    if(not std::filesystem::exists(path))
    {
        return RetBool(false, "Path does not exist: " + path.string());
    }
    search_path_ = path;
    return RetBool(true);
}


RetBool CodeDecoder::remove_comments()
{
    std::string line;
    std::regex comment_regex(R"((/\*.*\*/)|(/\*.*)|(.*\*/)|(//.*))");
    bool in_comment = false;

    std::string result;

    std::stringstream source_text_stream;
    source_text_stream << code_text_;

    while (std::getline(source_text_stream, line))
    {
        if (in_comment)
        {
            auto end_pos = line.find("*/");
            if (end_pos != std::string::npos)
            {
                in_comment = false;
                line.erase(0, end_pos + 2);
            }
            else
            {
                continue;
            }
        }

        std::smatch match;
        if (std::regex_search(line, match, comment_regex))
        {
            line.erase(match[0].first, match[0].second);

            if (match[2].matched)
            {
                in_comment = true;
            }
            else if(match[3].matched)
            {
                in_comment = false;
            }
        }

        result += line + "\n";
    }
    code_text_ = result;
    return RetBool(true);
}

RetBool CodeDecoder::scan_objects()
{
    // 清理缓存
    {
        import_package_list_.clear();
    }
    // 查找协议字段
    {
        std::regex re("proto.+");
        std::smatch match;
        std::string header_line;
        if(std::regex_search(code_text_, match, re))
        {
            object_->set_code_block(match.str());
            header_line = match.str();
        }

        // std::regex pre("\\S+$");
        // std::smatch pmatch;
        // if(std::regex_search(header_line, pmatch, pre))
        // {
        //     root_ = pmatch.str();
        // }
    }
    // 处理引用文件
    std::size_t head_size = 0;
    {
        /*
        (?:import\s+([a-zA-Z_][a-zA-Z0-9_]*)(?:\s+as\s+([a-zA-Z_][a-zA-Z0-9_]*))?(?:\s*,\s*[a-zA-Z_][a-zA-Z0-9_]*(?:\s+as\s+[a-zA-Z_][a-zA-Z0-9_]*)?)*)|(?:from\s+([a-zA-Z_][a-zA-Z0-9_.]*)\s+import\s+([a-zA-Z_][a-zA-Z0-9_.]*)(?:\s+as\s+([a-zA-Z_][a-zA-Z0-9_]*))?(?:\s*,\s*[a-zA-Z_][a-zA-Z0-9_.]*(?:\s+as\s+[a-zA-Z_][a-zA-Z0-9_]*)?)*)
        ((?:\bimport\s+([a-zA-Z_][a-zA-Z0-9_]*)(?:\s+as\s+([a-zA-Z_][a-zA-Z0-9_]*))?(?:\s*,\s*[a-zA-Z_][a-zA-Z0-9_]*(?:\s+as\s+[a-zA-Z_][a-zA-Z0-9_]*)?)*)|(?:\bfrom\s+([a-zA-Z_][a-zA-Z0-9_.]*)\s+import\s+([a-zA-Z_][a-zA-Z0-9_.]*)(?:\s+as\s+([a-zA-Z_][a-zA-Z0-9_]*))?(?:\s*,\s*[a-zA-Z_][a-zA-Z0-9_.]*(?:\s+as\s+[a-zA-Z_][a-zA-Z0-9_]*)?)*))
        上述正则表达式处理类似 python 的引入方式：
        import abc
        from abc import data, abc, data.data
        from abc.f import data, time
        import panda as pd
        */
        std::regex re(R"((?:\bimport\s+([a-zA-Z_][a-zA-Z0-9_]*)(?:\s+as\s+([a-zA-Z_][a-zA-Z0-9_]*))?(?:\s*,\s*[a-zA-Z_][a-zA-Z0-9_]*(?:\s+as\s+[a-zA-Z_][a-zA-Z0-9_]*)?)*)|(?:\bfrom\s+([a-zA-Z_][a-zA-Z0-9_.]*)\s+import\s+([a-zA-Z_][a-zA-Z0-9_.]*)(?:\s+as\s+([a-zA-Z_][a-zA-Z0-9_]*))?(?:\s*,\s*[a-zA-Z_][a-zA-Z0-9_.]*(?:\s+as\s+[a-zA-Z_][a-zA-Z0-9_]*)?)*))");
        std::string_view text = code_text_;
        for(std::match_results<std::string_view::const_iterator> sm; std::regex_search(text.begin(), text.end(), sm, re);)
        {
            ImportPackage pkg;
            // 匹配 from 字段
            try {
                std::smatch pkg_sm;
                std::regex pkg_re(R"(\bfrom\s+\S+)");
                const std::string pkg_text = sm.str();
                if(std::regex_search(pkg_text, pkg_sm, pkg_re))
                {
                    const std::string key("from");
                    auto start = pkg_sm.str().find_first_of(key);
                    pkg.from = coin::strings::remove_leading_chars(pkg_sm.str().substr(start + key.size()), " ");
                }
            }
            catch(const std::regex_error& e)
            {
                std::cerr << e.what() << std::endl;
            }
            // 匹配 import 字段
            try {
                std::smatch pkg_sm;
                std::regex pkg_re(R"(import\s+([^,\s]+(?:\s*,\s*[^,\s]+)*))");
                const std::string pkg_text = sm.str();
                if(std::regex_search(pkg_text, pkg_sm, pkg_re))
                {
                    const std::string key("import");
                    auto start = pkg_sm.str().find_first_of(key);
                    pkg.import = coin::strings::split(pkg_sm.str().substr(start + key.size()),  ",");
                    // 移除前向空格
                    for(auto itor = pkg.import.begin(); itor != pkg.import.end(); itor)
                    {
                        if(itor->empty())
                        {
                            itor = pkg.import.erase(itor);
                        }
                        else
                        {
                            *itor = coin::strings::remove_leading_chars(*itor, " ");
                            ++itor;
                        }
                    }
                }
            }
            catch(const std::regex_error& e)
            {
                std::cerr << e.what() << std::endl;
            }
            // 匹配 as 字段
            try {
                std::smatch pkg_sm;
                std::regex pkg_re(R"(\bas\s+\S+)");
                const std::string pkg_text = sm.str();
                if(std::regex_search(pkg_text, pkg_sm, pkg_re))
                {
                    const std::string key("as");
                    auto start = pkg_sm.str().find_first_of(key);
                    pkg.as = coin::strings::remove_leading_chars(pkg_sm.str().substr(start + key.size()), " ");
                }
            }
            catch(const std::regex_error& e)
            {
                std::cerr << e.what() << std::endl;
            }
            import_package_list_.emplace_back(std::move(pkg));
            text.remove_prefix(sm.position() + sm.str().size());
            head_size += sm.position() + sm.str().size();
        }
    }

    process_code(code_text_.substr(head_size), "", object_);

    return RetBool();
}
const std::string CodeDecoder::trans_type2path(std::string type)
{
    auto pos = type.find("::");
    while(pos != std::string::npos)
    {
        type.replace(pos, 2, ".");
        pos = type.find("::", pos + 1);
    }
    return canonicalize_path_name(type);
}
const std::string CodeDecoder::trans_path2type(std::string path)
{
    const std::string from("/");
    const std::string to(".");
    auto pos = path.find(from);
    while(pos != std::string::npos)
    {
        path.replace(pos, 1, to);
        pos = path.find(from, pos + 1);
    }
    return (path);
}

const std::string CodeDecoder::canonicalize_path_name(const std::string& p)
{
    auto path = "." + p;
    std::regex re(R"(\.+)");
    path = std::regex_replace(path, re, ".");
    return path;
}
const std::string CodeDecoder::get_full_type(const std::string& type) const
{
    std::string t = type;
    const std::string path = trans_type2path(t);

    for(auto& decoder : import_decoder_list_)
    {
        auto itor = decoder.second->find(path);
        if(itor != decoder.second->mate_object_end_)
        {
            t = itor->second->path() + itor->second->name();
            Print::debug_yes("find type <{}> from decoder: <{}>", t, decoder.first);
            break;
        }
    }
    return trans_path2type(t);
}
RetBool CodeDecoder::check_is_valid(const std::shared_ptr<CodeTreeConfig>& config, const std::map<std::string, std::shared_ptr<const CodeDecoder>> &import_decoder)
{
    // 检查引入的 frame 是否合理，如果合理则加入类型清单
    std::map<std::string, ImportPackage> import_map;
    for(auto& import : import_package_list_)
    {
        if(import.import.empty())
        {
            std::stringstream ss;
            ss << "invalid package import: " << import;
            Print::error(ss.str());
            return RetBool(false, ss.str());
        }
        for(auto& im : import.import)
        {
            import_map.emplace(im, import);
        }
    }

    auto is_datatype_valid = [this, &import_decoder, &config, &import_map](const std::string& type) -> RetBool {
    
        RetBool result(true, "");

        // 数据类型是否合法
        auto type_list = CodeTypeTree::calculate_type_list(type);

        for(auto& type : type_list) do
        {
            // 检查是否为默认类型
            {
                auto find_import = type_tree_->default_type_map().find(type);
                if(find_import != type_tree_->default_type_map().end())
                {
                    break;
                }
            }
            // 检查是否为本包中的 frame 定义
            {
                auto t = trans_type2path(type);
                auto find_type = this->find(t);
                if(find_type != this->mate_object_end_)
                {
                    if(find_type->second->get_mate_type() == MateObject::MateType::MATE_FRAME)
                    {
                        break;
                    }
                }
            }
            // 检查是否为注册类型
            {
                std::string type_str(type);
                std::string def_import_type = "";
                auto pos = type_str.find_last_of("::");
                auto find_import = import_map.end();
                do
                {
                    // 检查类型是否存在
                    auto t = trans_type2path(type_str);
                    find_import = import_map.find(t.substr(1));
                    if(find_import != import_map.end())
                    {
                        break;
                    }
                    pos = type_str.find_last_of("::");
                    def_import_type = "." + type_str.substr(pos + 1) + def_import_type;
                    type_str = type_str.substr(0, pos - 1);
                } while(pos != std::string::npos);

                if(find_import == import_map.end())
                {
                    result = false;
                    result = "<" + type + "> is not import, please check.";
                    break;
                }
                // 检查类型的合法性
                {
                    auto type_check_info = read_path_and_package(config, find_import->second.from, find_import->first + def_import_type);
                    if(type_check_info.first.empty())
                    {
                        result = false;
                        result += "can not find import package <" + find_import->first + ">";
                        break;
                    }
                    auto find_decoder = import_decoder.find(type_check_info.first);
                    if(find_decoder == import_decoder.end())
                    {
                        result = false;
                        result += "<" + type_check_info.first + "> is not a valid type.";
                        break;
                    }
                    auto find_type = find_decoder->second->find(type_check_info.second);
                    if(find_type == find_decoder->second->mate_object_end_)
                    {
                        result = false;
                        result += "<" + type_check_info.first + "/" + type_check_info.second + "> is not a valid type.";
                        break;
                    }
                }
            }
        } while(false);
        return result;
    };

    // 检查全部 field
    RetBool result(true, "invalid field type: ");
    foreach<GrammarField>(object_, [this, &is_datatype_valid, &result](const MateObject::Ptr mate, std::shared_ptr<const GrammarField> gitem, const MateObject::Ptr top) {
        auto ret = is_datatype_valid(std::string( gitem->type() ));
        if(not ret)
        {
            result = false;
            result += ret.reason();
        }
    });
    if(not result)
    {
        return result;
    }

    // 检查全部 communicator
    result = RetBool(true, "invalid communicator type: ");
    foreach<GrammarCommunicator>(object_, [this, &is_datatype_valid, &result](const MateObject::Ptr mate, std::shared_ptr<const GrammarCommunicator> gitem, const MateObject::Ptr top){
        Print::debug("####check communicator: {}####", gitem->name());
        Print::debug("comm type: {}", gitem->comm_type());
        Print::debug("data type: {}", gitem->data_type());
        // 检查通信类型是否合法
        do {
            // 检查通信类型
            auto comm_itor = type_tree_->communicator_type_map().find(gitem->comm_type());
            if(comm_itor == type_tree_->communicator_type_map().end())
            {
                result = false;
                result += std::string(gitem->path()) + "/" + std::string(gitem->name()) + ":" + std::string(gitem->comm_type()) + "\n";
                break;
            }
            // 检查数据类型
            auto data_ret = is_datatype_valid(gitem->data_type());
            if(not data_ret)
            {
                result = false;
                result += data_ret.reason();
                break;
            }
        } while(0);
    });
    if(not result)
    {
        return result;
    }
    return RetBool(true);
}
std::map<std::string, MateObject::Ptr>::iterator CodeDecoder::find(const std::string &name) const
{
    std::map<std::string, MateObject::Ptr>::iterator find_itor = mate_object_end_;
    std::string_view name_view(name);
    auto cobject = object_;

    auto pos = name_view.find_first_of(".");
    do
    {
        auto end_pos = name_view.find_first_of(".", pos + 1);
        std::string_view key_view = name_view.substr(pos + 1, end_pos - pos - 1);
        find_itor = cobject->find(key_view);
        if(find_itor == cobject->end())
        {
            return mate_object_end_;
        }
        cobject = find_itor->second;
        
        auto remvoe_pos = end_pos == std::string_view::npos ? name_view.size() : end_pos;
        name_view.remove_prefix(remvoe_pos);
        pos = name_view.find_first_of(".");
    } while(pos != std::string_view::npos);

    return find_itor;
}
std::size_t CodeDecoder::process_code(const std::string_view &code, const std::string &header, MateObject::Ptr obj)
{
    /**
     * @brief 正则匹配，匹配满足以下格式的字符串
     * 1. frame message{ ... };
     * 2. int a: 1 =10 @[0, 10];
     */
    static const std::string key_words_regex(R"((((\bframe)+\s+.+(\s?\{))|(\}\s*;))|((\w|::)+(<(\w|\s|,|<|>)+>)?\s+.+;))");
    // 此处通过递归调用来解析代码
    // 正则表达式匹配 code 中的全部关键词
    std::regex re(key_words_regex);
    std::string code_item;
    std::size_t idx = 0;
    std::string block = header;
    for(idx = 0; idx < code.size(); idx)
    {
        // 按照关键词匹配代码块
        std::match_results<std::string_view::const_iterator> key_words_match;
        std::string_view code_sub = code.substr(idx);
        bool res = std::regex_search(code_sub.begin(), code_sub.end(), key_words_match, re);

        // 如果没有任何匹配则返回
        if(not res)
        {
            return code.size();
        }

        // '{' 表示代码块的起始，'}' 表示代码块的结束
        std::size_t is_front = key_words_match.str().find('{');
        if(is_front != std::string::npos)
        {
            idx = key_words_match.position() + key_words_match.str().size() + idx;
            MateObject::Ptr mate = std::make_shared<MateObject>();
            mate->set_mate_type(MateObject::MateType::MATE_FRAME);
            mate->set_code_block(key_words_match.str() + "};");
            if(obj)
            {
                obj->append(mate);
            }
            idx += process_code(code.substr(idx), "", mate);
            continue;
        }
        std::size_t is_tail = key_words_match.str().find('}');
        if(is_tail != std::string::npos)
        {
            idx = key_words_match.position() + key_words_match.str().size() + idx;
            return idx;
        }
        auto has_sem = key_words_match.str().find(';');
        if(has_sem != std::string::npos)
        {
            {
                MateObject::Ptr mate = std::make_shared<MateObject>();
                mate->set_mate_type(MateObject::MateType::MATE_FIELD);
                mate->set_code_block(key_words_match.str());
                obj->append(mate);
            }
            idx = key_words_match.position() + key_words_match.str().size() + idx;
            continue;
        }
    }
    return idx;
}
void CodeDecoder::foreach (const MateObject::Ptr mate, 
    const std::function<void(const MateObject::Ptr, std::shared_ptr<const GrammarBase>, const MateObject::Ptr)> &func)
{
    MateObject::foreach(mate,
    [&](const MateObject::Ptr mate, const MateObject::Ptr top) {
        auto maker = key_word_factory_map.find(mate->code_type());
        if(maker != key_word_factory_map.end())
        {
            func(mate, maker->second(mate), top);
        }
    });
}
void CodeDecoder::foreach(const MateObject::Ptr mate,
    const std::function<void(const MateObject::Ptr, std::shared_ptr<const GrammarBase>, const MateObject::Ptr)>& enter,
    const std::function<void(const MateObject::Ptr, std::shared_ptr<const GrammarBase>, const MateObject::Ptr)>& leave
    )
{
    MateObject::foreach(mate,
    [&](const MateObject::Ptr mate, const MateObject::Ptr top) {
        auto maker = key_word_factory_map.find(mate->code_type());
        if(maker != key_word_factory_map.end())
        {
            enter(mate, maker->second(mate), top);
        }
    },
    [&](const MateObject::Ptr mate, const MateObject::Ptr top) {
        auto maker = key_word_factory_map.find(mate->code_type());
        if(maker != key_word_factory_map.end())
        {
            leave(mate, maker->second(mate), top);
        }
    }
    );
}

const std::pair<std::string, std::string> CodeDecoder::calculate_valid_path(const std::string &path, const std::string& file, const std::string& file_suffix)
{
    // 使用 path 和 file 拼接完整路径，并逐步回退file的路径，直到找到一个合法文件
    std::string_view check_file = file;
    std::string pkg_path;
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
                return {std::filesystem::weakly_canonical(result).string(), pkg_path};
            }
            else
            {
                return {"", ""};
            }
        }
        // 不存在则回退一层
        pos = check_file.find_last_of("/");
        if(pos == std::string::npos) break;
        pkg_path = "." + std::string(check_file.substr(pos + 1)) + pkg_path;
        check_file.remove_suffix(check_file.size() - pos);
        result = std::filesystem::weakly_canonical(path) / (std::string(check_file) + file_suffix);
    }
    return {"", ""};
}
const std::pair<std::string, std::string> CodeDecoder::read_path_and_package(const std::shared_ptr<CodeTreeConfig>& config, const std::string& from, const std::string& import)
{
    auto path = import;
    std::replace(path.begin(), path.end(), '.', '/');
    if(not from.empty())
    {
        auto from_path = from;
        std::replace(from_path.begin(), from_path.end(), '.', '/');
        path = from_path + "/" + path;
    }

    std::pair<std::string, std::string> import_type_info{"", path};
    for(auto& it : config->all_search_dirs())
    {
        // 计算有效绝对路径
        import_type_info = calculate_valid_path(it, path, config->suffix);
        if(not import_type_info.first.empty())
        {
            // 非空则有效，退出循环
            break;
        }
    }
    return std::move(import_type_info);
}
const std::vector<std::pair<std::string, std::string>> CodeDecoder::read_import_path_and_package(const std::shared_ptr<CodeTreeConfig>& config, const CodeDecoder::ImportPackage& pkg)
{
    std::vector<std::pair<std::string, std::string>> import_file_list;
    for(auto& itor : pkg.import)
    {
        auto import_file = read_path_and_package(config, pkg.from, itor);
        import_file_list.push_back(import_file);
    }
    return import_file_list;
}

std::ostream &operator<<(std::ostream &os, const CodeDecoder::ImportPackage &pkg)
{
    if(not pkg.from.empty())
    {
        os << "from: <" << pkg.from << ">";
    }
    if(not pkg.import.empty())
    {
        os << " import: ";
        for(const auto& i : pkg.import)
        {
            os << "<" << i << "> ";
        }
    }
    if(not pkg.as.empty())
    {
        os << "as: <" << pkg.as << ">";
    }
    return os;
}
} // namespace coin::code
