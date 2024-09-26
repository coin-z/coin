/**
 * @brief obj_gen.cpp
*/

#include "commc.hpp"
#include <filesystem>
#include <toml.hpp>
#include <iostream>
#include <getopt.h>
#include <algorithm>

#include <map>
#include <vector>

static std::string calCommType(const std::string_view& c)
{
    const std::map<std::string_view, std::string> comm_map{
        {"<publisher>", "MutablePublisher"},
        {"<subscriber>", "MutableSubscriber"},
        {"<reader>", "MutableReader"},
        {"<writer>", "MutableWriter"},
        {"<client>", "Client"},
        {"<server>", "Server"},
    };
    auto ret = comm_map.find(c);
    if(ret != comm_map.end())
    {
        return ret->second;
    }
    return std::string(c);
}

static std::string path2Class(const std::string_view& c)
{
    std::string ret;
    std::string line;
    std::stringstream ss;
    ss << c;

    while(std::getline(ss, line, '/'))
    {
        ret += line + "::";
    }
    return ret;
}

static std::string removeWhitespace(const std::string_view& str)
{
    std::string s(str);
    s.erase(std::remove_if(s.begin(), s.end(), [](char c){
        return std::isspace(c);
    }));
    return s;
}

namespace coin
{
    
ObjGen::ObjGen() : is_drymodel(false)
{
}

ObjGen::~ObjGen()
{

}

bool ObjGen::loadFile(const std::string &file, const std::string tail)
{
    // 检查文件是否存在，不存在则创建
    std::filesystem::path fs_file(file);
    if(not std::filesystem::exists(fs_file))
    {
        return false;
    }

    std::vector<std::string> files_process;

    // 如果是文件则直接解析传入的文件并保存，如果是路径则查找该路径下的全部带有指定尾缀的文件
    auto fs = std::filesystem::status(file);
    if(std::filesystem::is_directory(fs))
    {
        for(auto &f : std::filesystem::directory_iterator(fs_file))
        {
            if(f.is_regular_file())
            {
                auto fstr = f.path().string();
                if(fstr.substr(fstr.size() - tail.size()) == tail)
                {
                    files_process.push_back(f.path().string());
                }
            }
        }
    }
    else if(std::filesystem::is_regular_file(fs))
    {
        files_process.push_back(file);
    }
    else
    {
        return false;
    }

    // 开始处理文件
    proto_decoder_.clear();
    for(auto &itor : files_process)
    {
        auto pd = std::make_shared<coin::code::CodeDecoder>();
        pd->load_source_file(itor);
        pd->remove_comments();
        pd->scan_objects();
        proto_decoder_.push_back(pd);
    }


    return true;
}

bool ObjGen::loadFile(const std::vector<std::string> &files)
{
    // 开始处理文件
    proto_decoder_.clear();
    for(auto &itor : files)
    {
        auto pd = std::make_shared<coin::code::CodeDecoder>();
        {
            auto ret = pd->load_source_file(itor);
            if(not ret)
            {
                std::cout << ret.reason() << std::endl;
                return false;
            }

        }
        pd->remove_comments();
        pd->scan_objects();

        proto_decoder_.push_back(pd);
    }
    return true;
}

bool ObjGen::saveObjFile(const std::string &path)
{
    if(proto_decoder_.empty())
    {
        return false;
    }

    // 检查文件是否存在，如果不存在则创建
    if(not std::filesystem::exists(std::filesystem::path(path + "/" + target)))
    {
        std::filesystem::create_directories(std::filesystem::path(path + "/" + target));
    }

    for(auto& pd : proto_decoder_)
    {
        std::vector<std::shared_ptr<const coin::code::GrammarCommunicator> > proto_item;
        coin::code::CodeDecoder::foreach<coin::code::GrammarCommunicator>(pd->object_map(), 
        [&](const coin::code::MateObject::Ptr obj, 
            std::shared_ptr<const coin::code::GrammarCommunicator> gitem, 
            const coin::code::MateObject::Ptr top) {
            proto_item.emplace_back(gitem);
        });

        std::sort(proto_item.begin(), proto_item.end(), 
        [](std::shared_ptr< const coin::code::GrammarCommunicator >& a, std::shared_ptr< const coin::code::GrammarCommunicator >& b)->bool{
            return (a->path() < b->path());
        });


        std::filesystem::path source(pd->path());
        std::string name = source.filename();
        std::ofstream cppfile(path + "/" + target + "/" + name + ".cpp");
        std::ofstream hppfile(path + "/" + target + "/" + name + ".hpp");
        // 生成头文件内容
        {
            auto obj = pd->parse<coin::code::GrammarCommunicator>();
            const std::string header = 
R"(#include <coin-data.hpp>
#include <communicator_type.hpp>
#include <communicator.hpp>

)";
            hppfile << header;
            cppfile << "#include \"" + name + ".hpp\"\n";
        }

        std::string currentClass = pd->root();
        hppfile << "namespace " << pd->root() << " {" << std::endl;
        for(auto itor = proto_item.begin(); itor != proto_item.end(); itor++)
        {
            auto cpath = (*itor)->path();
            if(currentClass.empty())
            {
                currentClass = cpath.substr(0, cpath.find('/'));
            }
            while(true) {
                auto re_str = "^" + currentClass + "\\/.+";
                std::regex re(re_str);
                if(currentClass + "/" == cpath)
                {
                    hppfile << "    static coin::data::" << calCommType((*itor)->comm_type()) << "<" << (*itor)->data_type() << ">::Ptr" << (*itor)->name() << ";\n";
                    cppfile << "coin::data::" << calCommType((*itor)->comm_type()) << "<" << (*itor)->data_type() << ">::Ptr "\
                    << path2Class( (*itor)->path() ) << removeWhitespace((*itor)->name()) << " = "\
                    << "coin::data::" << calCommType((*itor)->comm_type()) << "<" << (*itor)->data_type() << ">::create(\"" << (*itor)->path() << removeWhitespace((*itor)->name()) << "\");\n";
                    break;
                }
                else if(std::regex_match(cpath.begin(), cpath.end(), re))
                {
                    std::stringstream pathss;
                    std::string p;
                    pathss << cpath.substr(currentClass.size() + 1);
                    while(std::getline(pathss, p, '/'))
                    {
                        if(currentClass != cpath)
                        {
                            currentClass += "/" + p;
                            hppfile << "struct " << p << " {" << std::endl;
                        }
                    }
                    hppfile << "    static coin::data::" << calCommType((*itor)->comm_type()) << "<" << (*itor)->data_type() << ">::Ptr" << (*itor)->name() << ";\n";
                    cppfile << "coin::data::" << calCommType((*itor)->comm_type()) << "<" << (*itor)->data_type() << ">::Ptr "\
                    << path2Class( (*itor)->path() ) << removeWhitespace((*itor)->name()) << " = "\
                    << "coin::data::" << calCommType((*itor)->comm_type()) << "<" << (*itor)->data_type() << ">::create(\"" << (*itor)->path() << removeWhitespace((*itor)->name()) << "\");\n";
                    break;
                }
                else
                {
                    currentClass = currentClass.substr(0, currentClass.find_last_of('/'));
                    hppfile << "};" << std::endl;
                    if(currentClass.empty())
                    {
                        break;
                    }
                }
            }
        }
        while(currentClass != pd->root())
        {
            hppfile << "};" << std::endl;
            currentClass = currentClass.substr(0, currentClass.find_last_of('/'));
        }
        hppfile << "} // " << pd->root() << std::endl;
        std::cout << path << "/" << target << "/" << name << ".cpp" << std::endl;
    }

    return true;
}

bool ObjGen::dumpObjFile(const std::vector<std::string> &files, const std::string &path)
{
    for(auto& f : files)
    {
        std::string name = f.substr(f.find_last_of("/"));
        std::cout << path << "/" << target << "/" << name << ".cpp" << std::endl;
    }
    return true;
}

} // namespace coin

int main(int argc, char *argv[])
{
    coin::ObjGen og;
    std::vector< std::string > inputs;
    std::string output;
    std::string input;
    std::stringstream input_stream;
    int opt = 0;
    while((opt = getopt(argc, argv, "t:i:o:d")) != -1)
    {
        switch (opt)
        {
        case 'i':
            input = std::string(optarg);
            break;
        case 'o':
            output = std::string(optarg);
            break;
        case 'd':
            og.is_drymodel = true;
            break;
        case 't':
            og.target = std::string(optarg);
        default:
            break;
        }
    }

    std::replace(input.begin(), input.end(), ';', ' ');

    input_stream << input;

    std::string line;

    while(std::getline(input_stream, line, ' '))
    {
        inputs.push_back(line);
    }

    if(og.is_drymodel)
    {
        if(not og.dumpObjFile(inputs, output))
        {
            return -1;
        }
    }
    else
    {
        if(not og.loadFile(inputs))
        {
            return -1;
        }

        if(not og.saveObjFile(output))
        {
            return -1;
        }
    }

    return 0;
}

