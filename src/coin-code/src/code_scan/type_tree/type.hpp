/**
 * @file type.hpp
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

namespace coin::code
{
class TypeBase
{
public:
    explicit TypeBase(const std::string& name);
    virtual ~TypeBase() = default;

    const std::string& name() const { return type_name_; }

private:
    std::string type_name_;
};

class TypeInt : public TypeBase
{
public:
    explicit TypeInt();
    virtual ~TypeInt() = default;
};
class TypeInt8 : public TypeBase
{
public:
    explicit TypeInt8();
    virtual ~TypeInt8() = default;
};
class TypeInt16 : public TypeBase
{
public:
    explicit TypeInt16();
    virtual ~TypeInt16() = default;
};
class TypeInt32 : public TypeBase
{
public:
    explicit TypeInt32();
    virtual ~TypeInt32() = default;
};
class TypeInt64 : public TypeBase
{
public:
    explicit TypeInt64();
    virtual ~TypeInt64() = default;
};
class TypeUInt8 : public TypeBase
{
public:
    explicit TypeUInt8();
    virtual ~TypeUInt8() = default;
};
class TypeUInt16 : public TypeBase
{
public:
    explicit TypeUInt16();
    virtual ~TypeUInt16() = default;
};
class TypeUInt32 : public TypeBase
{
public:
    explicit TypeUInt32();
    virtual ~TypeUInt32() = default;
};
class TypeUInt64 : public TypeBase
{
public:
    explicit TypeUInt64();
    virtual ~TypeUInt64() = default;
};
class TypeFloat : public TypeBase
{
public:
    explicit TypeFloat();
    virtual ~TypeFloat() = default;
};

class TypeDouble : public TypeBase
{
public:
    explicit TypeDouble();
    virtual ~TypeDouble() = default;
};

class TypeString : public TypeBase
{
public:
    explicit TypeString();
    virtual ~TypeString() = default;
};
class TypeBool : public TypeBase
{
public:
    explicit TypeBool();
    virtual ~TypeBool() = default;
};
class TypeEnum : public TypeBase
{
public:
    explicit TypeEnum();
    virtual ~TypeEnum() = default;
};
class TypeArray : public TypeBase
{
public:
    explicit TypeArray();
    virtual ~TypeArray() = default;
};
class TypeList : public TypeBase
{
public:
    explicit TypeList();
    virtual ~TypeList() = default;
};
class TypeMap : public TypeBase
{
public:
    explicit TypeMap();
    virtual ~TypeMap() = default;
};


class TypeCommunicatorWriter : public TypeBase
{
public:
    explicit TypeCommunicatorWriter();
    virtual ~TypeCommunicatorWriter() = default;
};
class TypeCommunicatorReader : public TypeBase
{
public:
    explicit TypeCommunicatorReader();
    virtual ~TypeCommunicatorReader() = default;
};
class TypeCommunicatorPublisher : public TypeBase
{
public:
    explicit TypeCommunicatorPublisher();
    virtual ~TypeCommunicatorPublisher() = default;
};
class TypeCommunicatorSubscriber : public TypeBase
{
public:
    explicit TypeCommunicatorSubscriber();
    virtual ~TypeCommunicatorSubscriber() = default;
};
class TypeCommunicatorService : public TypeBase
{
public:
    explicit TypeCommunicatorService();
    virtual ~TypeCommunicatorService() = default;
};
class TypeCommunicatorClient : public TypeBase
{
public:
    explicit TypeCommunicatorClient();
    virtual ~TypeCommunicatorClient() = default;
};
} // namespace coin::code
