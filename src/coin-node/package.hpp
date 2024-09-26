#pragma once
#include <string>
#include <vector>
#include <memory>

#include <coin-commons/parmeter/anydata.hpp>

#include <code_scan/code_decoder.hpp>

namespace coin::node
{

class PackageBaseInfo
{
public:
    using ProtocolMap = std::map<std::string, std::shared_ptr<coin::code::CodeDecoder>>;
    using ProtocolMapPtr = std::shared_ptr<ProtocolMap>;


    PackageBaseInfo() = delete;
    PackageBaseInfo(const PackageBaseInfo& rhs);
    PackageBaseInfo& operator=(const PackageBaseInfo&);
    ~PackageBaseInfo() = default;

    PackageBaseInfo(const std::string& name, const std::string& category, const std::vector<std::string>& tags, const std::string& description);

    inline std::string name() const { return __m_name; }
    inline std::string description() const { return __m_description; }
    inline std::string category() const { return __m_category; }
    inline std::vector<std::string> tags() const { return __m_tags; }

    void loadProtocolInfo(const std::string& path);

    std::string connectorInfoFile() const;
    ProtocolMapPtr connectorInfo() const;

    std::string url() const;


private:
    std::string __m_name;
    std::string __m_description;
    std::string __m_category;
    std::vector<std::string> __m_tags;

    ProtocolMapPtr __m_connector_info;

};

// class PackageDetailInfo : public PackageBaseInfo
// {
// public:
//     PackageDetailInfo() = delete;
//     PackageDetailInfo(const PackageDetailInfo& rhs);
//     PackageDetailInfo& operator=(const PackageDetailInfo&) = delete;
//     ~PackageDetailInfo() = default;

//     inline std::string version() const { return __m_version; }
//     inline std::string author() const { return __m_author; }
//     inline std::string email() const { return __m_email; }
//     inline std::string license() const { return __m_license; }
//     inline std::string url() const { return __m_url; }
//     inline std::vector<std::string> dependencies() const { return __m_dependencies; }
//     inline std::vector<std::string> build_dependencies() const { return __m_build_dependencies; }

// private:
//     std::string __m_version;
//     std::string __m_author;
//     std::string __m_email;
//     std::string __m_license;
//     std::string __m_url;
//     std::vector<std::string> __m_dependencies;
//     std::vector<std::string> __m_build_dependencies;

// };

class Package
{
public:
    using RetPackage = coin::Ret<std::shared_ptr<Package>>;

public:
    explicit Package(const PackageBaseInfo& info, const std::string& base_path);
    explicit Package(const std::string& base_path);
    Package() = delete;
    Package(const Package&) = delete;
    Package& operator=(const Package&) = delete;
    ~Package();

    RetPackage create();
    RetBool save();
    RetBool remove();

    std::string path();

    std::string command(const std::string& cmd);

    inline PackageBaseInfo& info() { return __m_info; }

    static RetBool isValide(const std::string& path);
    static RetBool isExist(const std::string& cate, const std::string& name, const std::string& ws_path);

    static std::string cate2Path(const std::string& cate);

    static std::vector<std::shared_ptr<Package>> scanPackage(const std::string& workspace);

private:

    std::shared_ptr<TomlAnyDataTable> __m_pkgInfoNode;
    PackageBaseInfo __m_info;
    const std::string __m_base_path;

    std::string __m_name;
    std::string __m_version;
    std::string __m_author;
    std::string __m_email;
    std::string __m_description;
    std::string __m_license;
    std::string __m_url;
    std::string __m_category;
    std::vector<std::string> __m_keywords;
    std::vector<std::string> __m_dependencies;
    std::vector<std::string> __m_build_dependencies;
    
    std::vector<std::string> __m_path_list;
    std::vector<std::pair<std::string, std::string>> __m_source_files;


};
} // namespace coin::node