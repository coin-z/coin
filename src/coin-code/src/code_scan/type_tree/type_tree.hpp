/**
 * @file type_tree.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include <string>
#include <vector>
#include <regex>
#include <map>
#include <memory>

#include <code_scan/type_tree/type.hpp>

namespace coin::code
{
class CodeTypeTree
{
public:
    using TypeTreeMap = std::map<std::string, std::shared_ptr<TypeBase>>;

    CodeTypeTree();
    virtual ~CodeTypeTree() = default;

    const TypeTreeMap& default_type_map() const;
    const TypeTreeMap& communicator_type_map() const;
    const TypeTreeMap& type_map() const;
    CodeTypeTree& regist_type(const std::string& name, std::shared_ptr<TypeBase> type);

    /**
     * @brief 拆解数据类型进行检查
     * 数据类型可以分为基本类型和复合类型
     * 对于基本类型形如：int float bool string
     * 对于复合类型形如：array<int> list<float>
     */
    static std::vector<std::string> calculate_type_list(const std::string& type);

private:

    const std::map<std::string, std::shared_ptr<TypeBase>> default_type_map_;
    const std::map<std::string, std::shared_ptr<TypeBase>> communicator_type_map_;
    std::map<std::string, std::shared_ptr<TypeBase>> type_map_;
};
} // namespace coin::code
