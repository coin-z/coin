/**
 * @file protocol_source_decoder.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once
#include <regex>
#include <vector>
#include <filesystem>
#include <iostream>

#include <code_scan/grammar/base.hpp>
#include <code_scan/grammar/communicator.hpp>
#include <code_scan/grammar/field.hpp>
#include <code_scan/grammar/frame.hpp>
#include <code_scan/config/code_tree.config.hpp>
#include <code_scan/config/code_exception.hpp>
#include <code_scan/type_tree/type_tree.hpp>

namespace coin::code
{
template<typename T>
struct Ret
{
    Ret() = default;
    Ret(const T& t) : value_(t) {}
    Ret(T&& t) : value_(std::move(t)) {}

    operator bool() const { return success_; }

private:
    bool success_;
    T value_;
public:
    friend std::ostream& operator << (std::ostream& os, const Ret<T>& ret)
    {
        os << "<" << (ret ? "true" : "false") << ">:" << ret.reason();
        return os;
    }
};

template<>
struct Ret<bool>
{
    Ret() = default;
    Ret(bool b, const std::string& reason = "") : success_(b), reason_(reason) {}
    Ret(bool b, std::string&& reason) : success_(b), reason_(std::move(reason)) {}
    Ret(Ret<bool>&& other) : success_(other.success_), reason_(std::move(other.reason_)) {}

    Ret& operator=(Ret<bool>&& other)
    {
        success_ = other.success_;
        reason_ = std::move(other.reason_);
        return *this;
    }

    Ret& operator=(const Ret<bool>& other)
    {
        success_ = other.success_;
        reason_ = other.reason_;
        return *this;
    }

    Ret& operator=(const bool b)
    {
        success_ = b;
        return *this;
    }

    Ret& operator=(const std::string& reason)
    {
        reason_ = reason;
        return *this;
    }

    Ret& operator+=(const std::string& reason)
    {
        reason_ += reason;
        return *this;
    }

    Ret& operator+(const std::string& reason)
    {
        reason_ += reason;
        return *this;
    }

    operator bool() const { return success_; }

    const std::string& reason() const { return reason_; }

private:
    bool success_ = false;
    std::string reason_;
public:
    friend std::ostream& operator << (std::ostream& os, const Ret<bool>& ret)
    {
        os << "<" << (ret ? "true" : "false") << ">:" << ret.reason();
        return os;
    }
};

using RetBool = Ret<bool>;

class CodeDecoder
{
public:
    CodeDecoder();
    virtual ~CodeDecoder();

    virtual RetBool load_source_file(const std::filesystem::path& path);
    virtual RetBool update_search_path(const std::filesystem::path& path);
    virtual RetBool remove_comments();
    virtual RetBool scan_objects();
    virtual RetBool check_is_valid(const std::shared_ptr<CodeTreeConfig>& config, const std::map<std::string, std::shared_ptr<const CodeDecoder>>& import_decoder);

    template<typename T> inline std::vector<typename T::Ptr> parse();

    inline const std::string path() const { return path_; }

    inline const std::string search_path() const { return search_path_; }

    const std::string get_full_type(const std::string& type) const;

    inline const std::string root() const { return root_; }

    inline const MateObject::Ptr& object_map() const { return object_; }

    inline void set_import_list(const std::map<std::string, std::shared_ptr<const CodeDecoder>>& list) { import_decoder_list_ = list; }
    inline std::map<std::string, std::shared_ptr<const CodeDecoder>>& import_list() { return import_decoder_list_; }

    template<typename GrammarT>
    static void foreach(const MateObject::Ptr mate,
        const std::function<void(const MateObject::Ptr, std::shared_ptr<const GrammarT>, const MateObject::Ptr)>& func);

    static void foreach(const MateObject::Ptr mate,
        const std::function<void(const MateObject::Ptr, std::shared_ptr<const GrammarBase>, const MateObject::Ptr)>& func);

    static void foreach(const MateObject::Ptr mate,
        const std::function<void(const MateObject::Ptr, std::shared_ptr<const GrammarBase>, const MateObject::Ptr)>& enter,
        const std::function<void(const MateObject::Ptr, std::shared_ptr<const GrammarBase>, const MateObject::Ptr)>& leave
        );

    static const std::map< std::string, std::function< GrammarBase::Ptr(const MateObject::Ptr) > > key_word_factory_map;

    std::map<std::string, MateObject::Ptr>::iterator find(const std::string& name) const;

private:
    std::filesystem::path path_;
    std::filesystem::path search_path_;
    std::stringstream source_text_stream_;
    std::string code_text_;
    std::string root_;

    MateObject::Ptr object_;

    std::map<std::string, MateObject::Ptr>::iterator mate_object_end_;

    std::map<std::string, std::shared_ptr<const CodeDecoder>> import_decoder_list_;

    std::shared_ptr<CodeTypeTree> type_tree_;

    std::size_t process_code(const std::string_view& code, const std::string& header = "", MateObject::Ptr obj = nullptr);

public:
    struct ImportPackage
    {
        std::string from;
        std::vector<std::string> import;
        std::string as;

        friend std::ostream& operator << (std::ostream& os, const coin::code::CodeDecoder::ImportPackage& pkg);

    };
    using ImportPackageList = std::vector<ImportPackage>;
    inline ImportPackageList& import_package_list() { return import_package_list_; }

    static const std::pair<std::string, std::string> read_path_and_package(const std::shared_ptr<CodeTreeConfig>& config, const std::string& from, const std::string& import);
    static const std::vector<std::pair<std::string, std::string>> read_import_path_and_package(const std::shared_ptr<CodeTreeConfig>& config, const ImportPackage& pkg);
    static const std::string trans_type2path(std::string type);
    static const std::string trans_path2type(std::string path);
    static const std::string canonicalize_path_name(const std::string& p);
private:
    static const std::pair<std::string, std::string> calculate_valid_path(const std::string &path, const std::string& file, const std::string& file_suffix);

private:
    ImportPackageList import_package_list_;
};

template <typename T>
inline std::vector<typename T::Ptr> CodeDecoder::parse()
{
    std::vector<typename T::Ptr> result;
    std::regex re("(" + T::code + "|" + T::code + "<.+>)");
    MateObject::foreach(object_, [&result, &re](const MateObject::Ptr mate, const MateObject::Ptr top)
    {
        std::match_results<std::string::const_iterator> matchs;
        if(std::regex_search(mate->key_word().begin(), mate->key_word().end(), matchs, re))
        {
            auto maker = key_word_factory_map.find(T::code);
            if(maker != key_word_factory_map.end())
            {
                result.emplace_back( std::static_pointer_cast<T>( maker->second(mate) ));
            }
        }
    });

    return std::move(result);
}

template <typename GrammarT>
inline void CodeDecoder::foreach (const MateObject::Ptr mate, 
    const std::function<void(const MateObject::Ptr, std::shared_ptr<const GrammarT>, const MateObject::Ptr)> &func)
{
    std::regex re(GrammarT::regex());
    auto maker = key_word_factory_map.find(GrammarT::code);
    MateObject::foreach(mate,
    [&](const MateObject::Ptr mate, const MateObject::Ptr top) {
        std::match_results<std::string::const_iterator> matchs;
        if(std::regex_search(mate->code_block().begin(), mate->code_block().end(), matchs, re))
        {
            if(maker != key_word_factory_map.end())
            {
                func(mate, std::static_pointer_cast<GrammarT>( maker->second(mate) ), top);
            }
        }
    });
}
} // namespace coin::code
