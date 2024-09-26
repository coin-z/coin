/**
 * @file command_processor.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "command_processor.hpp"

#include <magic.h>
#include <iostream>
#include <sstream>

#include <deque>

#include <cstdlib>
#include <cstring>
#include <unistd.h>


static std::string findCommand(const std::string& command)
{
    // 获取PATH环境变量
    char* path = std::getenv("PATH");
    if (path == nullptr) {
        std::cerr << "PATH environment variable not found" << std::endl;
        return "";
    }

    // 使用stringstream将path转换为string
    std::stringstream pathStream(path);

    // 使用getline分割路径
    std::string pathToken;
    while (std::getline(pathStream, pathToken, ':')) {
        // 构建完整的命令路径
        std::string commandPath = pathToken + "/" + command;

        // 使用access函数检查文件是否存在
        if (access(commandPath.c_str(), X_OK) == 0)
        {
            return std::move(commandPath);
        }
    }

    return "";
}


namespace coin::superpanel
{
CommandProcessor::CommandProcessor()
{
}

CommandProcessor::~CommandProcessor()
{
}

void CommandProcessor::setupProcessor(const std::string &name, const ProcessorType &func)
{
    __m_processor[name] = func;
}

void CommandProcessor::setupExecutorProcessor(const ProcessorType &func)
{
    __m_executorProcessor = func;
}

void CommandProcessor::setupUnknownProcessor(const ProcessorUnknownType &func)
{
    __m_unknownProcessor = func;
}

void CommandProcessor::invokeProcessor(const std::string &cmd)
{
    // 检查文件存在性
    std::string file = cmd.substr(0, cmd.find(' '));

    if(access(file.c_str(), F_OK) != 0)
    {
        file = findCommand(file);
    }
    if(file.empty())
    {
        if(__m_unknownProcessor)
        {
            __m_unknownProcessor(cmd, "");
        }
        return;
    }


    magic_t magicCookie = magic_open(MAGIC_MIME_TYPE);
    if (magicCookie == nullptr) {
        std::cerr << "Failed to initialize libmagic" << std::endl;
        return;
    }

    if (magic_load(magicCookie, nullptr) != 0) {
        std::cerr << "Failed to load magic database" << std::endl;
        magic_close(magicCookie);
        return;
    }

    const char* mimeType = magic_file(magicCookie, file.c_str());

    const std::string mimeTypeStr(mimeType);

    magic_close(magicCookie);

    auto it = __m_processor.find(mimeTypeStr.substr(0, mimeTypeStr.find('/')));
    if(it != __m_processor.end())
    {
        it->second(cmd);
    }
    else if(access(file.c_str(), X_OK) == 0)
    {
        if(__m_executorProcessor)
        {
            __m_executorProcessor(cmd);
        }
    }
    else
    {
        if(__m_unknownProcessor)
        {
            __m_unknownProcessor(cmd, mimeTypeStr);
        }
    }
}

}// namespace coin::superpanel