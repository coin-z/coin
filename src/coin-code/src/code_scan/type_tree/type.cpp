/**
 * @file type.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <code_scan/type_tree/type.hpp>

namespace coin::code
{
TypeBase::TypeBase(const std::string& name) : type_name_(name) {}
TypeInt::TypeInt() : TypeBase("int") {}
TypeInt8::TypeInt8() : TypeBase("int8") {}
TypeInt16::TypeInt16() : TypeBase("int16") {}
TypeInt32::TypeInt32() : TypeBase("int32") {}
TypeInt64::TypeInt64() : TypeBase("int64") {}
TypeUInt8::TypeUInt8() : TypeBase("uint8") {}
TypeUInt16::TypeUInt16() : TypeBase("uint16") {}
TypeUInt32::TypeUInt32() : TypeBase("uint32") {}
TypeUInt64::TypeUInt64() : TypeBase("uint64") {}
TypeFloat::TypeFloat() : TypeBase("float") {}
TypeDouble::TypeDouble() : TypeBase("double") {}
TypeString::TypeString() : TypeBase("string") {}
TypeBool::TypeBool() : TypeBase("bool") {}
TypeEnum::TypeEnum() : TypeBase("enum") {}
TypeArray::TypeArray() : TypeBase("array") {}
TypeList::TypeList() : TypeBase("list") {}
TypeMap::TypeMap() : TypeBase("map") {}

TypeCommunicatorWriter::TypeCommunicatorWriter() : TypeBase("writer") {}
TypeCommunicatorReader::TypeCommunicatorReader() : TypeBase("reader") {}
TypeCommunicatorPublisher::TypeCommunicatorPublisher() : TypeBase("publisher") {}
TypeCommunicatorSubscriber::TypeCommunicatorSubscriber() : TypeBase("subscriber") {}
TypeCommunicatorService::TypeCommunicatorService() : TypeBase("service") {}
TypeCommunicatorClient::TypeCommunicatorClient() : TypeBase("client") {}
} // namespace coin::code