/**
 * @file cpp_backend.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <code_backend/cpp_backend.hpp>
#include <filesystem>
#include <list>
#include <fstream>
#include <chrono>
#include <coin-commons/utils/utils.hpp>

static std::string format_type(std::string type, const std::string& f = ".")
{
    const std::string from(f);
    const std::string to("::");
    auto pos = type.find(from);
    while(pos != std::string::npos)
    {
        type.replace(pos, 1, to);
        pos = type.find(from, pos + 1);
    }
    return (type);
}

namespace coin::code
{

template<typename T>
std::string gen_range(const std::string& type, const std::string& text)
{
    return "";
}

CppBackend::CppBackend(const std::shared_ptr<CodeDecoder>& decoder, const std::string& output)
  : decoder_(decoder)
  , output_path_(std::filesystem::path(output).parent_path().string())
  , output_cpp_(output + ".cpp")
  , output_hpp_(output + ".hpp")
  , type_map_({
        {"int", nullptr},
        {"int8", [](const std::shared_ptr<const GrammarField>& g, const std::string&){return "int8_t"; }},
        {"int16", [](const std::shared_ptr<const GrammarField>& g, const std::string&){return "int16_t"; }},
        {"int32", [](const std::shared_ptr<const GrammarField>& g, const std::string&){return "int32_t"; }},
        {"int64", [](const std::shared_ptr<const GrammarField>& g, const std::string&){return "int64_t"; }},
        {"uint8", [](const std::shared_ptr<const GrammarField>& g, const std::string&){return "uint8_t"; }},
        {"uint16", [](const std::shared_ptr<const GrammarField>& g, const std::string&){return "uint16_t"; }},
        {"uint32", [](const std::shared_ptr<const GrammarField>& g, const std::string&){return "uint32_t"; }},
        {"uint64", [](const std::shared_ptr<const GrammarField>& g, const std::string&){return "uint64_t"; }},
        {"float", nullptr},
        {"double", nullptr},
        {"string", [](const std::shared_ptr<const GrammarField>& g, const std::string&){return "coin::data::ShmString"; }},
        {"bool", nullptr},
        {"enum", std::bind(&CppBackend::cvt_enum_type_, this, std::placeholders::_1, std::placeholders::_2)},
        {"array", [](const std::shared_ptr<const GrammarField>& g, const std::string&){ return "coin::data::ShmVector"; }},
        {"list", [](const std::shared_ptr<const GrammarField>& g, const std::string&){ return "coin::data::ShmList"; }},
        {"map", [](const std::shared_ptr<const GrammarField>& g, const std::string&){ return "coin::data::ShmMap"; }},
  })
  , range_map_({
        {"int", std::bind(gen_range<int>, "int", std::placeholders::_1)},
        {"int8", std::bind(gen_range<int8_t>, "int8_t", std::placeholders::_1)},
        {"int16", std::bind(gen_range<int16_t>, "int16_t", std::placeholders::_1)},
        {"int32", std::bind(gen_range<int32_t>, "int32_t", std::placeholders::_1)},
        {"int64", std::bind(gen_range<int64_t>, "int64_t", std::placeholders::_1)},
        {"uint8", std::bind(gen_range<uint8_t>, "uint8_t", std::placeholders::_1)},
        {"uint16", std::bind(gen_range<uint16_t>, "uint16_t", std::placeholders::_1)},
        {"uint32", std::bind(gen_range<uint32_t>, "uint32_t", std::placeholders::_1)},
        {"uint64", std::bind(gen_range<uint64_t>, "uint64_t", std::placeholders::_1)},
        {"float", std::bind(gen_range<float>, "float", std::placeholders::_1)},
        {"double", std::bind(gen_range<double>, "double", std::placeholders::_1)},
        {"string", [](const std::string&){return "std::string"; }},
        {"bool", nullptr},
        {"enum", nullptr},
        {"array", nullptr},
        {"list", nullptr},
        {"map", nullptr},
  })
{

}

void CppBackend::generate()
{
    // 检查 output_path 是否存在，不存在则创建
    if(not std::filesystem::exists(output_path_))
    {
        std::filesystem::create_directories(output_path_);
    }

    // 检查 output cpp/hpp 是否存在，不存在则创建，存在则清空内容
    std::ofstream ofs_cpp(output_cpp_);
    ofs_cpp.clear();

    std::ofstream ofs_hpp(output_hpp_);
    ofs_hpp.clear();

    Print::debug("generate hpp file: <{}>", output_hpp_);
    Print::debug("generate cpp file: <{}>", output_cpp_);


    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm timestamp = *std::localtime(&now_time);

    std::stringstream header;

    // 添加头部信息
    header << "/**" << std::endl;
    header << "* PLEASE DO NOT CHANGE THIS FILE" << std::endl;
    header << "* Create Date: " << std::put_time(&timestamp, "%Y-%m-%d %H:%M:%S") << std::endl;
    header << "*/" << std::endl;
    header << std::endl;

    ofs_hpp << header.str();
    ofs_cpp << header.str();

    ofs_hpp << "#pragma once" << std::endl;
    ofs_hpp << "#include <coin-data.hpp>" << std::endl;
    ofs_hpp << "#include <communicator_type.hpp>" << std::endl;
    ofs_hpp << "#include <communicator.hpp>" << std::endl;
    ofs_hpp << "#include <coin-code/coin-code.co.hpp>" << std::endl;

    // 添加头文件
    auto import_list = decoder_->import_list();
    for(auto& item : import_list)
    {
        auto inc_file = std::filesystem::relative(item.second->path(), item.second->search_path());
        ofs_hpp << "#include \"" << inc_file.string() << ".hpp\"" << std::endl;
    }
    ofs_hpp << std::endl;

    ofs_cpp << "#include \"" << output_hpp_.substr(output_path_.size() + 1) << "\"" << std::endl;
    ofs_cpp << std::endl;

    // 计算命名空间
    static auto format_namespace = [](const std::string& str) {
        std::string ns;
        ns = str.substr(0, str.find_last_of('.'));
        std::replace_if(ns.begin(), ns.end(), [](const char c) {
            if(c == '.') return true;
            if(c == '-') return true;
            return false;
        }, '_');
        return ns;
    };

    auto ns = format_namespace( decoder_->root() );

    ofs_hpp << "namespace " << ns << " { " << std::endl;
    ofs_cpp << "namespace " << ns << " { " << std::endl;

    std::list<std::map<std::string, std::string>> code_buffer_stack;

    // 通过 decoder 生成 cpp 文件
    CodeDecoder::foreach(decoder_->object_map(), 
        [&code_buffer_stack, &ofs_cpp, &ofs_hpp, this](const MateObject::Ptr mate, std::shared_ptr<const GrammarBase> gitem, const MateObject::Ptr top) {

            auto path = mate->path();
            path = path.substr(path.find_first_of("/") + 1);
            path = format_type(path, "/");
            auto type_path = format_type(path, "/");

            if(gitem->code_type() == GrammarBase::CodeType::FRAME)
            {
                std::map<std::string, std::string> code_buffer;
                
                code_buffer["constructor"] += type_path + mate->name() + "::" + mate->name() + "()\n";
                code_buffer["destructor"] += type_path + mate->name() + "::~" + mate->name() + "(){}\n";
                
                code_buffer_stack.push_back(code_buffer);
                
                ofs_hpp << "class " << mate->name() << std::endl;
                ofs_hpp << "{" << std::endl;
                ofs_hpp << "public:" << std::endl;
                ofs_hpp << std::string(4 * code_buffer_stack.size(), ' ') << mate->name() << "();" << std::endl;
                ofs_hpp << std::string(4 * code_buffer_stack.size(), ' ') << "~" << mate->name() << "();" << std::endl;
            }
            else if(gitem->code_type() == GrammarBase::CodeType::FIELD)
            {
                auto g = std::static_pointer_cast<const GrammarField>(gitem);

                auto type = gen_type_(decoder_, g, mate->code_block());
                auto type_name = get_type_name_(decoder_, g, mate->code_block());

                ofs_hpp << std::string(4 * code_buffer_stack.size(), ' ') << type << " " << g->name() << ";" << std::endl;

                auto& code_buffer = code_buffer_stack.back();
                code_buffer["init-list"] += "," + std::string(g->name()) + "(" + std::string(g->default_value()) + ")\n";

                /*
                if(not g->range().empty())
                {
                    ofs_hpp << "bool is_inrange(const " << type_name + "& value);" << std::endl;

                    code_buffer["range-check"] += "bool " + type_path + "is_inrange(const " + type_path + type_name + "& value)\n{\n";
                    code_buffer["range-check"] += "//" + std::string( g->range() ) + "\n";
                    code_buffer["range-check"] += "}\n";
                }
                */
            }
            else if(gitem->code_type() == GrammarBase::CodeType::COMMUNICATOR)
            {
                auto g = std::static_pointer_cast<const GrammarCommunicator>(gitem);

                auto& code_buffer = code_buffer_stack.back();
                std::string modifier = "static";
                if(path.empty())
                {
                    modifier = "extern";
                }
                {
                    ofs_hpp << std::string(4 * code_buffer_stack.size(), ' ') << modifier << " coin::data::" << cvt_communicator_type(decoder_, g, mate->code_block()) << "::Ptr " << g->name() << ";" << std::endl;
                    ofs_cpp << "coin::data::" + cvt_communicator_type(decoder_, g, mate->code_block()) << "::Ptr " \
                        << type_path + g->name() << " = coin::data::" \
                        << cvt_communicator_type(decoder_, g, mate->code_block()) << "::create(\"" + std::string(g->path()) + g->name() + "\");\n";
                }
            }
        },
        [&code_buffer_stack, &ofs_cpp, &ofs_hpp](const MateObject::Ptr mate, std::shared_ptr<const GrammarBase> gitem, const MateObject::Ptr top) {

            auto& code_buffer = code_buffer_stack.back();

            if(gitem->code_type() == GrammarBase::CodeType::FRAME)
            {
                if(not code_buffer["init-list"].empty())
                {
                    code_buffer["constructor"] += ":" + code_buffer["init-list"].substr(1);
                }
                // 清理 init-list
                code_buffer.erase("init-list");
                code_buffer["constructor"] += "{}\n";

                for(auto& code : code_buffer)
                {
                    ofs_cpp << code.second;
                }
                code_buffer.clear();
                code_buffer_stack.pop_back();

                ofs_hpp << std::string(4 * code_buffer_stack.size(), ' ') << "}; // class " << mate->name() << std::endl;
            }
        }
    );

    ofs_hpp << "} // namespace " << ns << std::endl;
    ofs_cpp << "} // namespace " << ns << std::endl;

}
void CppBackend::generate_cmake(const std::set<std::string>& inc_dirs, const std::string& output, const std::string& target)
{
    std::string file_name = output + "/" + "CMakeLists.txt";
    std::ofstream cmake(file_name);
    auto tab = [](const int n) -> const std::string {
        const std::string s(4 * n, ' ');
        return std::move(s);
    };
    cmake.clear();

    cmake << R"(CMAKE_MINIMUM_REQUIRED(VERSION 3.12.0)
PROJECT()";
    cmake << target;
    cmake << R"( VERSION 1.0.0)

# 开启 C++17支持
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS ON)

FIND_PACKAGE(Coin REQUIRED code data)

)";

    cmake << R"(FILE(GLOB_RECURSE CO_SRCS ${CMAKE_SOURCE_DIR} "*.co.cpp"))" << std::endl;

    cmake << "ADD_LIBRARY(${PROJECT_NAME} SHARED ${CO_SRCS})" << std::endl;

    cmake << "TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME}" << std::endl;
    cmake << "PRIVATE" << std::endl;
    cmake << tab(1) << "${CMAKE_SOURCE_DIR}" << std::endl;
    cmake << tab(1) << "${Coin_INCLUDE_DIR}" << std::endl;
    for(auto& dir : inc_dirs)
    {
        cmake << tab(1) << dir << std::endl;
    }
    cmake << ")" << std::endl;

    cmake << "TARGET_LINK_LIBRARIES(${PROJECT_NAME}" << std::endl;
    cmake << tab(1) << "${Coin_LIBRARIES}" << std::endl;
    cmake << ")" << std::endl;
    cmake << "SET(" << target << "_INCLUDE_DIR ";
    for(auto& dir : inc_dirs)
    {
        cmake << "\"${CMAKE_CURRENT_SOURCE_DIR}/" << dir << "\" ";
    }
    cmake << R"(CACHE INTERNAL "INCLUDE DIR OF CO TARGET")";
    cmake << ")" << std::endl;
}

std::string CppBackend::gen_type_(const std::shared_ptr<const CodeDecoder>& decoder, const std::shared_ptr<const GrammarField>& g, const std::string& code_block)
{
    std::size_t pos = 0;
    std::string cvt_type = "";
    std::string type(g->type());
    std::string buf;

    auto get_type = [&decoder, &g, &code_block, this](const std::string& type) {
        auto t = type;
        auto t_itor = type_map_.find(t);
        if(t_itor != type_map_.end())
        {
            if(t_itor->second)
            {
                t = t_itor->second(g, code_block);
            }
        }
        else if(not type.empty())
        {
            t = decoder->get_full_type(type);
            t = format_type(t);
        }
        return std::move(t);
    };
    for(auto& s : type)
    {
        if(s == '<' || s == '>' || s == ',' || s == ' ' || s == '\t' || s == '\r' || s == '\n')
        {
            auto t = get_type(buf);
            cvt_type += t + s;
            buf.clear();
        }
        else
        {
            buf += s;
        }
    }

    cvt_type += get_type(buf);;

    return std::move(cvt_type);
}

std::string CppBackend::get_type_name_(const std::shared_ptr<const CodeDecoder>& decoder, const std::shared_ptr<const GrammarField>& g, const std::string& code_block)
{
    std::size_t pos = 0;
    std::string cvt_type = "";
    std::string type(g->type());
    std::string buf;

    auto get_type = [&decoder, &g, &code_block, this](const std::string& type) {
        auto t = type;
        auto t_itor = type_map_.find(t);
        if(t_itor != type_map_.end())
        {
            if(t_itor->second)
            {
                t = t_itor->second(g, code_block);
            }
        }
        else if(not type.empty())
        {
            t = decoder->get_full_type(type);
            t = format_type(t);
        }
        return std::move(t);
    };
    for(auto& s : type)
    {
        if(s == '<' || s == '>' || s == ',' || s == ' ' || s == '\t' || s == '\r' || s == '\n')
        {
            
            std::string t;
            if(buf == "enum")
            {
                t = g->name();
                t[0] = std::toupper(t[0]);
                t = t + "EnumDef";
            }
            else
            {
                t = get_type(buf);
            }

            cvt_type += t + s;
            buf.clear();
        }
        else
        {
            buf += s;
        }
    }

    if(buf == "enum")
    {
        cvt_type = g->name();
        cvt_type[0] = std::toupper(cvt_type[0]);
        cvt_type = cvt_type + "EnumDef";
    }
    else
    {
        cvt_type = get_type(buf);
    }
    return std::move(cvt_type);
}

std::string CppBackend::cvt_enum_type_(const std::shared_ptr<const GrammarField>& g, const std::string& code)
{
    std::stringstream code_stream;

    std::string enum_name(g->name());
    enum_name += + "EnumDef";
    enum_name[0] = std::toupper(enum_name[0]);

    code_stream << "enum class " << enum_name << " {" << std::endl;
    std::string range(g->range());
    std::remove(range.begin(), range.end(), '[');
    auto rm_itor = std::remove(range.begin(), range.end(), ']');
    range.erase(rm_itor, range.end());
    code_stream << range << std::endl;
    code_stream << "};" << std::endl;
    code_stream << "enum " << enum_name;
    return code_stream.str();
}

std::string CppBackend::gen_type_(const std::shared_ptr<const CodeDecoder>& decoder, const std::shared_ptr<const GrammarCommunicator>& g, const std::string& code_block)
{
    std::size_t pos = 0;
    std::string cvt_type = "";
    std::string type(g->data_type());
    std::string buf;

    auto get_type = [&decoder, &g, &code_block, this](const std::string& type) {
        auto t = type;
        auto t_itor = type_map_.find(t);
        if(t_itor != type_map_.end())
        {
            if(t_itor->second)
            {
                t = t_itor->second(nullptr, code_block);
            }
        }
        else if(not type.empty())
        {
            t = decoder->get_full_type(type);
            t = format_type(t);
        }
        return std::move(t);
    };
    for(auto& s : type)
    {
        if(s == '<' || s == '>' || s == ',' || s == ' ' || s == '\t' || s == '\r' || s == '\n')
        {
            auto t = get_type(buf);
            cvt_type += t + s;
            buf.clear();
        }
        else
        {
            buf += s;
        }
    }

    cvt_type += get_type(buf);;

    return std::move(cvt_type);
}
std::string CppBackend::cvt_communicator_type(const std::shared_ptr<const CodeDecoder>& decoder, const std::shared_ptr<const GrammarCommunicator>& g, const std::string& code_block)
{
    const std::map<std::string_view, std::string> comm_map{
        {"publisher", "MutablePublisher"},
        {"subscriber", "MutableSubscriber"},
        {"reader", "MutableReader"},
        {"writer", "MutableWriter"},
        {"client", "Client"},
        {"server", "Server"},
    };
    auto type = g->comm_type();
    auto ret = comm_map.find(g->comm_type());
    if(ret != comm_map.end())
    {
        type = ret->second + "<" + gen_type_( decoder, g, code_block ) + ">";
    }
    return type;
}
} // namespace coin::code
