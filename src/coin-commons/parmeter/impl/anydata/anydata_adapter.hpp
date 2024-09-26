#pragma once

#include <memory>
#include <map>

namespace coin
{
template<typename ParserT> class AnyDataNode;
template<typename T, typename ParserT> class AnyDataItem;
template<typename ParserT> class AnyDataTable;

template<typename ParserT>
using NodePtr = std::shared_ptr<AnyDataNode<ParserT>>;

template<typename ParserT>
using NodeMap = std::map< std::string, NodePtr<ParserT> >;

template<typename ParserT>
using NodeMapPtr = std::shared_ptr< NodeMap<ParserT> >;

namespace anydata
{
template<typename T, typename ParserType>
struct AnyDataAdapter
{};
} // namespace anydata
} // namespace coin
