#include "mate_object.hpp"
#include <iostream>
#include <regex>

#include <coin-commons/utils/utils.hpp>
#include <coin-commons/utils/strings.hpp>

#include <code_scan/grammar/frame.hpp>

namespace coin::code
{
MateObject::MateObject() : parent_(nullptr)
{
}

void coin::code::MateObject::append(Ptr item)
{
    auto shared_from_this_ = shared_from_this();
    item->parent_ = shared_from_this_;
    item_list_.emplace_back(item);
    item_map_.emplace(std::string(item->name_), item);
}

void coin::code::MateObject::set_code_block(const std::string &code)
{
    code_block_ = code;
    std::string_view code_view = code_block_;

    auto get_head_key_word = [](std::string_view& sv) -> std::string {
        std::size_t flag_cnt = 0;
        auto pos = std::find_if(sv.begin(), sv.end(), [&flag_cnt](char c) {
            if(c == '<') flag_cnt += 1;
            if(c == '>') flag_cnt -= 1;
            if(flag_cnt == 0)
            {
                if(c == ' ' || c == '\t' || c == '\r' || c == '\n') return true;
            }
            else
            {
                return false;
            }

            return false;
        });
        std::string word(sv.substr(0, pos - sv.begin()));
        sv.remove_prefix(pos - sv.begin());
        return word;
    };

    // 提取关键字
    {
        key_word_ = get_head_key_word(code_view);
        code_view = coin::strings::remove_leading_chars(code_view, " ");
        name_ = get_head_key_word(code_view);
    }

    depict_ = code_view;

    if(key_word_ == "frame")
    {
        code_type_ = "frame";
    }
    else if(key_word_ == "proto")
    {

    }
    else if(key_word_.substr(0, key_word_.find_first_of('<')) == "communicator")
    {
        code_type_ = "communicator";
    }
    else
    {
        code_type_ = "field";
    }
}

const std::string &MateObject::code_block() const
{
    return code_block_;
}
const std::string &MateObject::code_type() const
{
    return code_type_;
}
const std::vector<MateObject::Ptr> &MateObject::item_list() const
{
    return item_list_;
}

const std::string& MateObject::key_word() const
{
    return key_word_;
}

const std::string& MateObject::name() const
{
    return name_;
}

const std::string& MateObject::depict() const
{
    return depict_;
}

const MateObject::Ptr MateObject::parent() const
{
    return parent_;
}
const std::string MateObject::path() const
{
    // 计算 path
    std::string path;
    auto p = parent();
    while(p)
    {
        path = std::string(p->name()) + "/" + path;
        p = p->parent();
    }
    return std::move(path);
}

std::map<std::string, MateObject::Ptr>& MateObject::item_map()
{
    return item_map_;
}
std::map<std::string, MateObject::Ptr>::iterator MateObject::find(const std::string& name)
{ return item_map_.find(name); }
std::map<std::string, MateObject::Ptr>::iterator MateObject::find(const std::string_view& name)
{
    return item_map_.find(std::string(name));
}
std::map<std::string, MateObject::Ptr>::iterator MateObject::end()
{ return item_map_.end(); }
static void foreachMate_(
    const MateObject::Ptr mate, 
    const MateObject::Ptr top,
    const std::function<void(const MateObject::Ptr mate, const MateObject::Ptr top)> &enter,
    const std::function<void(const MateObject::Ptr mate, const MateObject::Ptr top)> &leave = nullptr
)
{
    enter(mate, top);
    for(auto itor = mate->item_list().begin(); itor != mate->item_list().end(); itor++)
    {
        foreachMate_(*itor, top, enter, leave);
    }
    if(leave) leave(mate, top);
}

void MateObject::foreach (const Ptr mate, const std::function<void(const Ptr mate, const Ptr top)> &func)
{
    foreachMate_(mate, mate, func);
}
void MateObject::foreach(const Ptr mate, 
        const std::function<void(const Ptr mate, const Ptr top)>& enter,
        const std::function<void(const Ptr mate, const Ptr top)>& leave)
{
    foreachMate_(mate, mate, enter, leave);
}
} // namespace coin::code
