#include "system.hpp"
#include <stdlib.h>
#include <filesystem>
#include <fstream>

#include <toml.hpp>

namespace coin
{

SystemInfo::SystemInfo() : 
    __m_sysConfigPath("")
  , __m_sysConfigFile("system.toml")
  , __m_sysInfoTable("system")
  , __m_nodeListFile("node_list.toml")
  , __m_nodeListTable("nodes")
{
    // 生成配置信息
    __m_sysInfoTable.add<std::string>("root", ".");
    __m_sysInfoTable.add<std::string>("name", "coin");
    __m_sysInfoTable.add<std::string>("version", "v0.0.1");
    __m_sysInfoTable.add<std::string>("author", "xxx");
    __m_sysInfoTable.add<std::string>("email", "");
    __m_sysInfoTable.add<std::string>("description", "");
    __m_sysInfoTable.add<std::string>("license", "");
    __m_sysInfoTable.add<std::string>("url", "");
    __m_sysInfoTable.add("workspace", {
        coin::anydata::toml::Item<std::string>("path", ""),
        coin::anydata::toml::Item<std::string>("workspace", "")
    });
    __m_sysInfoTable.add("package_info", {

            anydata::toml::Node("cpp", {

                anydata::toml::Node("files", {

                    anydata::toml::Item<std::string>("base_path", ""),
                    anydata::toml::Item<std::vector<std::string>>("path_list", {
                        "src",
                        "doc",
                        "tools",
                        ".package",
                        "cmake",
                        "test"
                    }),
                    anydata::toml::Item<std::vector<std::string>>("source_files", {
                        "src/main.cpp",
                        "CMakeLists.txt",
                        "readme.md",
                        ".package/package.toml"
                    })

                }),

                anydata::toml::Node("codes", {
                    anydata::toml::Item<std::string>("src/main.cpp", R"(#include <iostream>

int main(int argc, char** argv)
{
    std::cout << "hello world!" << std::endl;
    return 0;
}
)"),
                    anydata::toml::Item<std::string>("CMakeLists.txt", R"(cmake_minimum_required(VERSION 3.16)
project(%%NAME%%)
set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS ON)

add_executable(${PROJECT_NAME} src/main.cpp)

)"),
                }),
            })
        })
        .add<std::vector<std::string>>("command", {
            "open: code .",
            "build: cmake -B build -S . && cmake --build build",
            "run: ./build/%%NAME%%",
            "package:"
        });

    __m_sysInfoTable.add("mainwindow", {
        anydata::toml::Item<int>("width", 800),
        anydata::toml::Item<int>("height", 600)
    });
    __m_sysInfoTable.add("superpanel", {
        anydata::toml::Item<int>("x", 0),
        anydata::toml::Item<int>("y", 0),
        anydata::toml::Item<int>("width", 400),
        anydata::toml::Item<int>("height", 600),
    });
    __m_sysInfoTable.add("focusepanel", {
        anydata::toml::Item<int>("x", 0),
        anydata::toml::Item<int>("y", 0),
        anydata::toml::Item<int>("width", 400),
        anydata::toml::Item<int>("height", 600),
    });

    // 检查系统配置目录是否存在，如果不存在则创建
    char* phome = getenv("HOME");
    if(phome)
    {
        __m_sysConfigPath = std::string(getenv("HOME")) + "/.config/coin/";
        // 检查配置文件目录是否存在
        if(not std::filesystem::exists(__m_sysConfigPath))
        {
            std::filesystem::create_directories(__m_sysConfigPath);
        }
        // 检查配置文件是否存在
        if(not std::filesystem::exists(__m_sysConfigPath + __m_sysConfigFile))
        {
            // 保存配置文件
            save();
        }
        else
        {
            // 加载配置文件
            load();
        }
    }
    

}

SystemInfo::~SystemInfo()
{
}

SystemInfo &SystemInfo::info()
{
    static SystemInfo ins;
    return ins;
}

RetBool SystemInfo::load()
{
    const std::string path = __m_sysConfigPath + __m_sysConfigFile;
    auto table = ::toml::parse_file(path);
    __m_sysInfoTable.decode(table);
    return RetBool(true);
}

RetBool SystemInfo::save()
{
    const std::string path = __m_sysConfigPath + __m_sysConfigFile;
    std::ofstream ofs(path);
    ::toml::table table;
    __m_sysInfoTable.encode(table);
    ofs << ::toml::toml_formatter{table};
    return RetBool(true);
}

TomlAnyDataTable &SystemInfo::root()
{
    return __m_sysInfoTable;
}

} // namespace coin
