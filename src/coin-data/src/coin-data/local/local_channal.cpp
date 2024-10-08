/**
 * @file local_channal.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include <coin-commons/utils/utils.hpp>
#include <coin-commons/utils/crc32.hpp>
#include <coin-commons/utils/datetime.hpp>
#include <coin-data/local/local_channal.hpp>

#include <signal.h>
#include <uuid/uuid.h>
#include <regex>

static void sys_exit(int s)
{
    exit(s);
}

namespace coin::data::local
{

Communicator::Communicator(const std::string &name) : name_(name)
{
}

Communicator::~Communicator()
{
}

LocalChannal::LocalChannal() : last_update_time_(0)
{
    loop_timer_.installTask(100 /*ms*/, std::bind(&LocalChannal::check_connect_status_, this));

    ShmObjManager::instance();

    // 生成节点名称，生成规则：
    // 1. 通过环境变量 COIN_NODE_NAME
    // 2. 通过 uuid 生成
    const char coin_node_name_key[] = "COIN_NODE_NAME";
    char* name_p = getenv(coin_node_name_key);
    if(name_p)
    {
        self_name_ = std::string(name_p);
    }
    else
    {
        uuid_t id;
        uuid_generate(id);
        char id_str[33];
        for(int i = 0; i < 16; i++)
        {
            std::sprintf(&id_str[i * 2], "%02X", id[i]);
        }
        id_str[32] = '\0';
        self_name_ = std::string(id_str);
    }

    // 初始化 node map
    init_node_map_();

    // 节点名称管理
    {
        node_map_mutex_.lock();

        // 检查该名称是否已经存在
        auto itor = node_map_->get()->find(fromStdString(self_name_));
        if(itor != node_map_->get()->end())
        {
            // 检查该进程是否存在
            if(kill(itor->second.pid, 0) == 0)
            {
                std::cout << "node name already exists: " << toStdString(itor->first) << ", pid: " << itor->second.pid << std::endl;
                node_map_mutex_.unlock();
                sys_exit(0);
            }
            else
            {
                // 节点名称存在，更新 pid
                itor->second.pid = getpid();
            }
        }
        else
        {
            // 将自己加入 node map
            node_map_->get()->insert({
                fromStdString(self_name_), 
                NodeMapItem{getpid()}
            });
        }
        
        node_map_mutex_.unlock();
    }

}

void LocalChannal::spin_once()
{
    loop_timer_.exec(coin::DateTime::current_date_time().to_msecs_since_epoch());

    for(auto itor = connected_list_.begin(); itor != connected_list_.end(); itor)
    {
        // 如果此时仅有容器中的引用，则代表外部已销毁，则放弃
        if(itor->use_count() == 1)
        {
            itor = connected_list_.erase(itor);
            continue;
        }

        // 调用工作状态的执行方法：invoke_、spin_
        if((*itor)->isReady_())
        {
            (*itor)->invoke_();
        }
        itor->get()->spin_();

        // 向下迭代
        itor++;
    }

    // 执行 work list 中的对象
    for(auto itor = work_list_.begin(); itor != work_list_.end(); itor)
    {
        if(itor->use_count() > 1 && itor->get()->isOnline())
        {
            itor->get()->spin_();
            itor ++;
        }
        else
        {
            std::cout << "remove from work list: " << (*itor)->name() << std::endl;
            itor = work_list_.erase(itor);
        }
    }
}

void LocalChannal::check_connect_immediately_()
{
    for(auto itor = wait_connect_list_.begin(); itor != wait_connect_list_.end(); itor)
    {
        // 如果没有外部引用则此处也放弃
        if(itor->use_count() == 1)
        {
            itor = wait_connect_list_.erase(itor);
            continue;
        }

        // 如果出现可用连接，则将其加入已连接列表
        if(ShmObjManager::instance().hasSharedObject((*itor)->name()))
        {
            connected_list_.push_back(*itor);
            (*itor)->connecteTo_();
            itor = wait_connect_list_.erase(itor);
        }
        else
        {
            itor++;
        }
    }

    // 更新时间
    last_update_time_ = ShmObjManager::instance().getSharedObjMapUpdateTime();
}

void LocalChannal::check_connect_status_()
{
    uint64_t update_time = ShmObjManager::instance().getSharedObjMapUpdateTime();
    if(last_update_time_ != update_time)
    {
        check_connect_immediately_();
        last_update_time_ = update_time;
    }

    for(auto itor = connected_list_.begin(); itor != connected_list_.end(); itor)
    {
        if(not (*itor)->isOnline())
        {
            wait_connect_list_.push_back(*itor);
            (*itor)->disconnect_();
            itor = connected_list_.erase(itor);
        }
        else
        {
            itor++;
        }
    }
}

void LocalChannal::init_node_map_()
{
    ProcessLockGuard<ProcessMutex> lock(node_map_mutex_);
    node_map_ = ShmObjManager::instance().create<ShmMap<ShmString, NodeMapItem>>("$$node_map");
}

LocalChannal::~LocalChannal()
{
    ProcessLockGuard<ProcessMutex> lock(node_map_mutex_);
    node_map_->get()->erase( fromStdString(self_name_) );
}

LocalChannal &LocalChannal::instance()
{
    static LocalChannal ins;
    return ins;
}

void LocalChannal::init(int argc, char* argv[])
{

}

const std::map<std::string, std::string>& LocalChannal::communicators()
{
    auto load_communicators_from_env = []()->std::map<std::string, std::string>{
        std::map<std::string, std::string> node_communicators;
        const char coin_node_communications_key[] = "COIN_NODE_COMMUNICATIONS";
        char* communications_str = getenv(coin_node_communications_key);
        if(communications_str)
        {
            auto str = std::string(communications_str);
            if(str.empty())
            {
                return node_communicators;
            }
            std::regex re("\\s*;+\\s*");
            std::sregex_token_iterator it(str.begin(), str.end(), re, -1);
            std::sregex_token_iterator end;
            while(it != end)
            {
                std::string s = it->str();
                std::regex re_blank("\\s*:+\\s*");
                std::sregex_token_iterator it_blank(s.begin(), s.end(), re_blank, -1);
                std::sregex_token_iterator end_blank;

                const std::string& key = it_blank->str();
                const std::string& value = (++it_blank)->str();
                node_communicators.insert_or_assign(key, value);

                it++;
            }
        }
        return node_communicators;
    };
    static std::map<std::string, std::string> comms = load_communicators_from_env();
    return comms;
}


LocalChannal::LoopTimer::LoopTimer()
{
}

LocalChannal::LoopTimer::~LoopTimer()
{
}

void LocalChannal::LoopTimer::installTask(const uint64_t& cycle, const std::function<void()>& task) noexcept
{
    task_list_.push_back({cycle, 0, task});
}

void LocalChannal::LoopTimer::exec(const uint64_t& time) noexcept
{
    for(auto itor = task_list_.begin(); itor != task_list_.end(); itor++)
    {
        if(time - itor->last_invoke_time > itor->cycle)
        {
            itor->task();
            itor->last_invoke_time = time;
        }
    }
}

} // namespace coin::data
