/**
 * @file type_tree.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <code_scan/type_tree/type_tree.hpp>
#include <coin-commons/utils/utils.hpp>
#include "type_tree.hpp"

namespace coin::code
{
CodeTypeTree::CodeTypeTree()
    : default_type_map_{
          {"int", std::make_shared<TypeInt>()},
          {"int8", std::make_shared<TypeInt8>()},
          {"int16", std::make_shared<TypeInt16>()},
          {"int32", std::make_shared<TypeInt32>()},
          {"int64", std::make_shared<TypeInt64>()},
          {"uint8", std::make_shared<TypeUInt8>()},
          {"uint16", std::make_shared<TypeUInt16>()},
          {"uint32", std::make_shared<TypeUInt32>()},
          {"uint64", std::make_shared<TypeUInt64>()},
          {"float", std::make_shared<TypeFloat>()},
          {"double", std::make_shared<TypeDouble>()},
          {"string", std::make_shared<TypeString>()},
          {"bool", std::make_shared<TypeBool>()},
          {"enum", std::make_shared<TypeEnum>()},
          {"array", std::make_shared<TypeArray>()},
          {"list", std::make_shared<TypeList>()},
          {"map", std::make_shared<TypeMap>()},
    },
    communicator_type_map_{
        {"writer", std::make_shared<TypeCommunicatorWriter>()},
        {"reader", std::make_shared<TypeCommunicatorReader>()},
        {"publisher", std::make_shared<TypeCommunicatorPublisher>()},
        {"subscriber", std::make_shared<TypeCommunicatorSubscriber>()},
        {"service", std::make_shared<TypeCommunicatorService>()},
        {"client", std::make_shared<TypeCommunicatorClient>()},
    }
{ }

const CodeTypeTree::TypeTreeMap &CodeTypeTree::default_type_map() const
{
    return default_type_map_;
}
const CodeTypeTree::TypeTreeMap &CodeTypeTree::communicator_type_map() const
{
    return communicator_type_map_;
}
const CodeTypeTree::TypeTreeMap& CodeTypeTree::type_map() const
{
    return type_map_;
}
CodeTypeTree& CodeTypeTree::regist_type(const std::string& name, std::shared_ptr<TypeBase> type)
{
    // 检查是否已经存在，如果已经出现，则不再注册
    auto itor = type_map_.find(name);
    if(itor == type_map_.end())
    {
        type_map_.insert(std::make_pair(name, type));
    }
    return *this;
}
std::vector<std::string> CodeTypeTree::calculate_type_list(const std::string& type)
{
    std::vector<std::string> type_list;
    std::regex re(R"([A-Za-z0-9_:]+)");
    
    std::string_view type_view(type);
    for(std::match_results<std::string_view::iterator> result; std::regex_search(type_view.begin(), type_view.end(), result, re);)
    {
        type_list.emplace_back(result.str());
        type_view.remove_prefix(result.position() + result.length());
    }

    return std::move(type_list);
};

} // namespace coin::code