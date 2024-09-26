#pragma once

#include <string>
#include <coin-commons/utils/ret.hpp>
#include <coin-commons/parmeter/anydata.hpp>

namespace coin
{

class SystemInfo
{
public:

    SystemInfo(const SystemInfo&) = delete;
    SystemInfo& operator=(const SystemInfo&) = delete;
    ~SystemInfo();

    static SystemInfo& info();

    RetBool load();
    RetBool save();

    template<typename T>
    T getParam(const std::string& key);

    TomlAnyDataTable& root();

    template<typename T>
    void setParam(const std::string& key, const T& value);

private:
    SystemInfo();
    std::string __m_sysConfigPath;
    const std::string __m_sysConfigFile;
    TomlAnyDataTable __m_sysInfoTable;

    const std::string __m_nodeListFile;
    TomlAnyDataTable __m_nodeListTable;

};

template <typename T>
inline T SystemInfo::getParam(const std::string &key)
{
    return __m_sysInfoTable[key].as<T>();
    // return std::forward( __m_sysInfoTable[key].as<T>() );
}

template <typename T>
inline void SystemInfo::setParam(const std::string &key, const T &value)
{
    *__m_sysInfoTable[key].access<T>() = value;
}

} // namespace coin
