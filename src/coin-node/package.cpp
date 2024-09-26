#include "package.hpp"
#include <filesystem>
#include <sstream>
#include <iostream>
#include <regex>
#include <cassert>

#include <coin-commons/utils/datetime.hpp>
#include <coin-node/system.hpp>

using namespace std::string_view_literals;

namespace coin::node
{

static std::shared_ptr< TomlAnyDataTable > makePackageInfoTable(const PackageBaseInfo& info = {"", "", {}, ""})
{
    using namespace coin::anydata;
    auto table = std::make_shared<TomlAnyDataTable>("root");
    table->add("node_info", {
            toml::Item<std::string>("name", info.name()),
            toml::Item<std::string>("category", info.category()),
            toml::Item<std::vector<std::string>>("tags", info.tags()),
            toml::Item<std::string>("description", info.description()),
            toml::Item<int64_t>("create_time", DateTime::current_date_time().to_msecs_since_epoch()),
            toml::Item<std::string>("version", "v0.0.1"),
            toml::Item<std::string>("author", "xxx"),
            toml::Item<std::string>("email", ""),
            toml::Item<std::string>("license", ""),
            toml::Item<std::string>("url", ""),
            toml::Item<std::vector<std::string>>("dependencies", {}),
            toml::Item<std::vector<std::string>>("build_dependencies", {})
        });

    return table;
}


Package::Package(const PackageBaseInfo& info, const std::string& base_path) : __m_pkgInfoNode(makePackageInfoTable(info))
  , __m_info(info)
  , __m_base_path(base_path)
{ }

Package::Package(const std::string &base_path) : __m_pkgInfoNode(makePackageInfoTable())
  , __m_info("", "", {}, "")
  , __m_base_path(base_path)
{
    // 从 package.toml 中载入项目信息
    auto pkgInfoPath = base_path + "/.package/package.toml";
    if(std::filesystem::exists(std::filesystem::path(pkgInfoPath)))
    {
        auto tomlInfo = ::toml::parse_file(pkgInfoPath);
        __m_pkgInfoNode->decode(tomlInfo);
        __m_info = PackageBaseInfo(
            (*__m_pkgInfoNode)["node_info.name"].as<std::string>(),
            (*__m_pkgInfoNode)["node_info.category"].as<std::string>(),
            (*__m_pkgInfoNode)["node_info.tags"].as< std::vector<std::string> >(),
            (*__m_pkgInfoNode)["node_info.description"].as<std::string>()
        );

        __m_info.loadProtocolInfo(base_path);
    }
    else
    {
        throw std::runtime_error("The package.toml file is not exist.");
    }
}

Package::~Package()
{ }

Package::RetPackage Package::create()
{
    RetPackage pkg;
    auto root_path = __m_base_path + "/";
    // 计算 category
    {
        std::stringstream ss;
        ss << __m_info.category();
        std::string item;
        while(std::getline(ss, item, '.'))
        {
            root_path += item + "/";
        }
    }
    // root_path += __m_info.name() + "/" + (*__m_pkgInfoNode)["package_info.cpp.files.base_path"].as<std::string>();
    root_path += __m_info.name() + "/";
    root_path += coin::SystemInfo::info().getParam<std::string>("package_info.cpp.files.base_path");
    // 不存在则创建
    if(not std::filesystem::exists(std::filesystem::path(root_path)))
    {
        std::filesystem::create_directories(std::filesystem::path(root_path));
    }
    else if(not std::filesystem::is_empty(std::filesystem::path(root_path)))
    {
        return pkg(false, "The root path is not empty.");
    }

    // 创建目录列表
    // auto path_list = (*__m_pkgInfoNode)["package_info.cpp.files.path_list"].as< std::vector<std::string> >();
    auto path_list = coin::SystemInfo::info().getParam< std::vector<std::string> >("package_info.cpp.files.path_list");
    for(auto& path : path_list)
    {
        std::filesystem::create_directories(std::filesystem::path(root_path) / path);
    }

    // 创建文件列表
    // auto file_list = (*__m_pkgInfoNode)["package_info.cpp.files.source_files"].as< std::vector<std::string> >();
    auto file_list = coin::SystemInfo::info().getParam< std::vector<std::string> >("package_info.cpp.files.source_files");
    for(auto& file : file_list)
    {
        std::filesystem::path file_path = std::filesystem::path(root_path) / file;
        std::filesystem::path ppath = file_path.parent_path();
        if(not std::filesystem::is_directory(ppath))
        {
            std::filesystem::create_directories(ppath);
        }
        std::ofstream file_stream(file_path);
        file_stream.close();
    }

    // 写入模板文本
    // auto& codes = (*__m_pkgInfoNode)["package_info.cpp.codes"];
    auto& codes = coin::SystemInfo::info().root()["package_info.cpp.codes"];
    codes.walking([&](const TomlAnyDataNode& node) {

        if(not node.is_node())
        {
            std::filesystem::path file_path = std::filesystem::path(root_path) / node.key();
            std::ofstream file_stream(file_path);
            // 替换模板变量
            std::string text = node.as<std::string>();
            std::regex reg("%%NAME%%");
            text = std::regex_replace(text, reg, __m_info.name());
            file_stream << text;
            file_stream.close();
        }

    });

    // 保存包信息到文件
    std::filesystem::path pkgFilePath = std::filesystem::path(root_path) / ".package/package.toml";
    std::ofstream pkgInfoFile(pkgFilePath);
    ::toml::table pkgInfoToml;
    __m_pkgInfoNode->encode(pkgInfoToml);
    auto text = ::toml::toml_formatter(pkgInfoToml);
    pkgInfoFile << text;

    return pkg;
}

RetBool Package::save()
{
    // 保存包信息到文件
    std::filesystem::path pkgFilePath = std::filesystem::path(this->path()) / ".package/package.toml";
    std::ofstream pkgInfoFile(pkgFilePath);
    ::toml::table pkgInfoToml;
    __m_pkgInfoNode->encode(pkgInfoToml);
    auto text = ::toml::toml_formatter(pkgInfoToml);
    pkgInfoFile << text;
    return RetBool(true);
}

RetBool Package::remove()
{
    // 删除
    auto root_path = __m_base_path + "/";
    // 计算 category
    {
        std::stringstream ss;
        ss << __m_info.category();
        std::string item;
        while(std::getline(ss, item, '.'))
        {
            root_path += item + "/";
        }
    }
    root_path += __m_info.name();

    // 删除自己
    std::filesystem::remove_all(std::filesystem::path(root_path));

    // 如果父目录里仅有当前子目录，那么将父目录也删除，直到工作控件停止检查
    auto parent_path = std::filesystem::path(root_path).parent_path();
    while(parent_path.string() != __m_base_path)
    {
        auto iter = std::filesystem::directory_iterator(parent_path);
        if(iter == std::filesystem::directory_iterator())
        {
            // 保证不可以删除 __m_base_path 以外的文件
            assert(parent_path.string().find(__m_base_path) == 0);
            std::filesystem::remove_all(parent_path);
        }
        else
        {
            break;
        }
        parent_path = parent_path.parent_path();
    }

    return RetBool();
}

std::string Package::path()
{
    // 计算包目录
    auto root_path = __m_base_path + "/";
    root_path += cate2Path(__m_info.category()) + "/";
    root_path += __m_info.name();

    return root_path;
}

std::string Package::command(const std::string &cmd)
{
    // 计算包目录
    auto path_list = coin::SystemInfo::info().getParam<std::vector< std::string > >("command");

    // 查找 command
    const std::string patt = "^" + cmd;
    std::regex reg(patt);
    for(auto &itor : path_list)
    {

        // 检查 itor 的首部是否为命令字符
        if(std::regex_search(itor, reg))
        {
            // 提取分号以后的内容作为 cmd，并移除开始的空格
            auto str = itor.substr(itor.find(":", 0) + 1);
            return str.substr(str.find_first_not_of(" "));
        }

    }

    return std::string();
}

RetBool Package::isValide(const std::string& root_path)
{
    // 检查当前目录是否为合法包
    auto pkgFilePath = std::filesystem::path(root_path) / ".package/package.toml";
    if(not std::filesystem::exists(pkgFilePath))
    {
        return RetBool(false, "The " + pkgFilePath.string() + " file is not exist.");
    }

    // 检查目录结构是否符合 package.toml 中的描述
    auto path_list = coin::SystemInfo::info().getParam< std::vector<std::string> > ("package_info.cpp.files.path_list");
    for(auto& path : path_list)
    {
        if(not std::filesystem::exists(std::filesystem::path(root_path + "/" + path)))
        {
            return RetBool(false, "The path <" + path + "> is not exist.");
        }
    }

    auto file_list = coin::SystemInfo::info().getParam< std::vector<std::string> > ("package_info.cpp.files.source_files");
    for(auto& file : file_list)
    {
        if(not std::filesystem::exists(std::filesystem::path(root_path + "/" + file)))
        {
            return RetBool(false, "The file <" + file + "> is not exist.");
        }
    }

    return RetBool(true);
}

RetBool Package::isExist(const std::string &cate, const std::string &name, const std::string &ws_path)
{
    // 计算包目录
    auto root_path = ws_path + "/";
    root_path += cate2Path(cate) + "/";
    root_path += name;

    // 检查该目录是否存在
    if(std::filesystem::exists(root_path))
    {
        return RetBool(true, "The package <" + cate + "." + name + "> is already exist.");
    }
    return RetBool(false);
}

std::string Package::cate2Path(const std::string &cate)
{
    std::string path = cate;
    std::replace(path.begin(), path.end(), '.', '/');
    return path;
}

std::vector<std::shared_ptr<Package>> Package::scanPackage(const std::string &workspace)
{
    // 递归子目录查找合法包
    std::vector<std::shared_ptr<Package>> packages;
    std::function<void(const std::string&)> scan = [&](const std::string& path)
    {
        auto iter = std::filesystem::directory_iterator(path);
        for(auto& item : iter)
        {
            if(std::filesystem::is_directory(item))
            {
                auto ret = Package::isValide(item.path().string());
                if(ret)
                {
                    auto pkg = std::make_shared<Package>(item.path().string());
                    packages.push_back(pkg);
                }
                else
                {
                    scan(item.path().string());
                }
            }
        }
    };
    scan(workspace);

    return packages;
}

PackageBaseInfo::PackageBaseInfo(const PackageBaseInfo &rhs)
    : __m_name(rhs.__m_name), __m_category(rhs.__m_category), __m_tags(rhs.__m_tags), __m_description(rhs.__m_description)
    , __m_connector_info(rhs.__m_connector_info)
{
}

PackageBaseInfo &PackageBaseInfo::operator=(const PackageBaseInfo &rhs)
{
    this->__m_category = rhs.__m_category;
    this->__m_description = rhs.__m_description;
    this->__m_name = rhs.__m_name;
    this->__m_tags = rhs.__m_tags;
    return *this;
}

PackageBaseInfo::PackageBaseInfo(const std::string &name, const std::string &category, const std::vector<std::string> &tags, const std::string &description)
    : __m_name(name), __m_category(category), __m_tags(tags), __m_description(description), __m_connector_info(std::make_shared<ProtocolMap>())
{
}

std::string PackageBaseInfo::connectorInfoFile() const
{
    return Package::cate2Path(__m_category) + "/" + __m_name + "/.package/connector.toml";
}

void PackageBaseInfo::loadProtocolInfo(const std::string& path)
{
    // 扫描全部 coin.protocol 文件，递归子目录
    std::filesystem::path directoryPath(path);

    // Iterate through all the files in the directory
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath))
    {
        // Check if the file extension is ".protocol"
        if(not std::filesystem::is_directory(entry))
        {
            
            std::regex re(R"(..+\.co$)");
            std::smatch m;
            if(std::regex_match(entry.path().string(), re))
            {
                auto decoder = std::make_shared<coin::code::CodeDecoder>();
                auto ret = decoder->load_source_file(entry.path().string());
                ret = decoder->remove_comments();
                decoder->scan_objects();

                __m_connector_info->emplace(
                    std::filesystem::relative(entry.path(), directoryPath).string(),
                    decoder
                );
            }

        }
    }
}

PackageBaseInfo::ProtocolMapPtr PackageBaseInfo::connectorInfo() const
{
    return __m_connector_info;
}

std::string PackageBaseInfo::url() const
{
    return Package::cate2Path(__m_category) + "/" + __m_name;
}

} // namespace coin::node