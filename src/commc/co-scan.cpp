#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <regex>

#include <code_scan/code_decoder.hpp>
#include <code_tree/code_tree.hpp>

#include <coin-commons/utils/utils.hpp>

#include <argparse/argparse.hpp>

using namespace coin::code;
int main(int argc, char** argv) {
    coin::init(argc, argv);

    argparse::ArgumentParser cmd("coin-code");
    cmd.add_argument("--input").help("input file").nargs(argparse::nargs_pattern::any);
    cmd.add_argument("--output").help("output directory");
    cmd.add_argument("--search").help("search path").nargs(argparse::nargs_pattern::any).default_value({""});
    cmd.add_argument("--target").help("build target");

    try{
        cmd.parse_args(argc, argv);
    }
    catch(const std::exception& err)
    {
        coin::Print::error(err.what());
        return -1;
    }

    coin::Print::debug("output: {}", cmd.get<std::string>("--output"));
    coin::Print::debug("search: {}", cmd.get<std::string>("--search"));
    coin::Print::debug("target: {}", cmd.get<std::string>("--target"));
    coin::Print::debug("input list:");
    for(auto& f : cmd.get<std::vector<std::string>>("--input"))
    {
        coin::Print::debug("    {}", f);
    }

    CodeTree tree;
    auto input_file_list = cmd.get<std::vector<std::string>>("--input");
    
    if(cmd.is_used("--search"))
    {
        auto search_dirs = cmd.get<std::vector<std::string>>("--search");
        if(not search_dirs.empty())
        {
            for(auto dir = search_dirs.begin(); dir != search_dirs.end(); dir)
            {
                if(not std::filesystem::exists(*dir))
                {
                    dir = search_dirs.erase(dir);
                    coin::Print::warn("search dir <{}> not exists", *dir);
                    continue;
                }
                else
                {
                    dir++;
                }
            }
            tree.regist_search_path(search_dirs);
        }
    }

    try{
        tree.set_target(cmd.get<std::string>("--target"));
    } catch(const std::exception& err)
    {
        coin::Print::error(err.what());
        return -1;
    }
    try {
        tree.load_code_file_list(input_file_list);
    } catch(const std::exception& err)
    {
        coin::Print::error(err.what());
        return -1;
    }
    try {
        tree.generate_target(input_file_list, cmd.get<std::string>("--output"));
    } catch(const std::exception& err)
    {
        coin::Print::error(err.what());
        return -1;
    }

    return 0;
}