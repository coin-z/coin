/**
 * @file command_processor.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <string>
#include <map>
#include <functional>


namespace coin::superpanel
{

class CommandProcessor
{
public:
    using ProcessorType = std::function<void(const std::string&)>;
    using ProcessorUnknownType = std::function<void(const std::string&, const std::string&)>;
public:
    CommandProcessor();
    ~CommandProcessor();

    void setupProcessor(const std::string& name, const ProcessorType& func);
    void setupExecutorProcessor(const ProcessorType& func);
    void setupUnknownProcessor(const ProcessorUnknownType& func);

    void invokeProcessor(const std::string& cmd);

private:
    std::map<std::string, ProcessorType> __m_processor;
    ProcessorType __m_executorProcessor;
    ProcessorUnknownType __m_unknownProcessor;

};

}// namespace coin::superpanel