/**
 * @file mate_object.hpp
*/

#pragma once
#include <memory>
#include <vector>
#include <map>
#include <functional>

namespace coin::code
{

class MateObject : public std::enable_shared_from_this<MateObject>
{

public:
    using Ptr = std::shared_ptr<MateObject>;

    enum class MateType {
        MATE_Unknown = 0,
        MATE_FRAME = 1,
        MATE_FIELD = 2,
    };

    MateObject();
    virtual ~MateObject() = default;

    inline virtual std::string to_string() const { return code_block_; };

    void append(Ptr item);
    void set_code_block(const std::string& code);

    const std::string& code_block() const;
    const std::vector<Ptr>& item_list() const;

    const std::string& key_word() const;
    const std::string& name() const;
    const std::string& depict() const;

    const std::string& code_type() const;

    const std::string path() const;

    const Ptr parent() const;

    static void foreach(const Ptr mate, const std::function<void(const Ptr mate, const Ptr top)>& func);
    static void foreach(const Ptr mate, 
        const std::function<void(const Ptr mate, const Ptr top)>& enter,
        const std::function<void(const Ptr mate, const Ptr top)>& leave);

    std::map<std::string, Ptr>::iterator find(const std::string_view& name);
    std::map<std::string, Ptr>::iterator find(const std::string& name);
    std::map<std::string, Ptr>::iterator end();
    std::map<std::string, Ptr>& item_map();

    void set_mate_type(const MateType& mate_type) { mate_type_ = mate_type; }
    MateType get_mate_type() const { return mate_type_; }

private:

    std::string code_block_;
    std::string code_type_;
    std::string key_word_;
    std::string name_;
    std::string depict_;
    Ptr parent_;
    std::vector<Ptr> item_list_;
    std::map<std::string, Ptr> item_map_;

    MateType mate_type_;


public:
    template<typename OStream>
    friend OStream& operator<<(OStream& os, const MateObject& obj)
    {
        os << obj.to_string();
        return os;
    }

};

} // namespace coin::code
