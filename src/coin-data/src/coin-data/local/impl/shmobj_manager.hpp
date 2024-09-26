/**
 * @file shmobj_manager.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once
#include <any>
#include <map>
#include <sys/types.h>
#include <unistd.h>

#include <coin-data/local/impl/shm_types.hpp>
#include <coin-data/local/impl/obj_allocator.hpp>
#include <coin-commons/utils/file_lock.hpp>
#include <coin-commons/utils/utils.hpp>
#include <coin-commons/utils/datetime.hpp>
#include <coin-commons/utils/type.hpp>

namespace coin::data
{

class ShmObjManager;

struct ShmObjInfo {
    ShmObjInfo() = default;
    ~ShmObjInfo() = default;

    void* obj_ptr = nullptr;
    ShmString obj_type;
    std::size_t hash_code;
    __inner::ShmObjList<pid_t> owener_list;
};
constexpr static uint64_t shm_system_magic_key = 0x12345678ABCDEF00;

#pragma pack(push, 4)
struct ShmMemSystemControl
{
    using ShmMemSystemObjMap = __inner::ShmObjMap<__inner::ShmObjString, ShmObjInfo>;
    using ShmMemSystemProcMap = __inner::ShmObjMap<pid_t, uint64_t>;

    ShmMemSystemControl();
    ~ShmMemSystemControl();

    inline const uint64_t getUpdateTime() const noexcept { return update_time_->load(std::memory_order_relaxed); }
    inline void setUpdateTime() noexcept { update_time_->store(
        coin::DateTime::current_date_time().to_msecs_since_epoch(), std::memory_order_relaxed); }

    uint64_t magic_value = shm_system_magic_key;
    coin::data::__inner::ShmMemory::ProcessMutex mutex;
    ShmMemSystemObjMap* shm_obj_map_;
    ShmMemSystemProcMap* shm_proc_map_;
    std::atomic< uint64_t >* update_time_;
    uint32_t crc;
};
#pragma pack(pop)

template<typename T>
class SharedObject : public std::enable_shared_from_this<SharedObject<T>>
{
friend class ShmObjManager;
private:
    template<typename U, typename... Args>
    T* newShmObject(Args&&... args)
    {
        coin::data::__inner::Allocator<U> alloc;
        auto ptr = alloc.allocate(1);
        alloc.construct(ptr, std::forward<Args>(args)...);
        return ptr;
    }

    struct PrivateCreate
    {
        PrivateCreate() = default;
    };

    struct PrivateAttach
    {
        PrivateAttach() = default;
    };

public:
    template<typename... ArgsT>
    SharedObject(const PrivateCreate&, const std::string& name, ArgsT&&... args) 
      : name_(name), obj_ptr_( newShmObject<T>(std::forward<ArgsT>(args)...) )
    { }

    SharedObject(const PrivateAttach&, const std::string& name, T* raw_ptr) : name_(name), obj_ptr_(raw_ptr)
    { }

    SharedObject() = delete;
    SharedObject(const SharedObject&) = delete;
    SharedObject(SharedObject&&) = delete;
    void operator ()() const = delete;
    SharedObject& operator = (const SharedObject&) = delete;
    SharedObject& operator = (SharedObject&&) = delete;

    virtual ~SharedObject()
    {
        if (obj_ptr_)
        {
            ProcessLockGuard<ProcessMutex> lock(shm_mem_sys_control_->mutex);

            auto itor = shm_mem_sys_control_->shm_obj_map_->find(__inner::ShmObjString(name_));
            if(itor != shm_mem_sys_control_->shm_obj_map_->end())
            {
                auto it = std::find_if(itor->second.owener_list.begin(), itor->second.owener_list.end(), [&](pid_t pid){ return pid == getpid(); });
                if(it != itor->second.owener_list.end())
                {
                    itor->second.owener_list.erase(it);
                }
                
                if(itor->second.owener_list.size() == 0)
                {
                    shm_mem_sys_control_->shm_obj_map_->erase(itor);
                    shm_mem_sys_control_->setUpdateTime();

                    coin::data::__inner::Allocator<T> alloc;
                    alloc.destroy(obj_ptr_);
                    alloc.deallocate(obj_ptr_, 1);
                }
            }
        }
    }

    T* get() const
    {
        return obj_ptr_;
    }

    template<typename... ArgsT>
    static std::shared_ptr<SharedObject<T>> create(const std::string& name, ArgsT&&... args)
    {
        return std::make_shared<SharedObject<T>>(PrivateCreate(), name, std::forward<ArgsT>(args)...);
    }

    static std::shared_ptr<SharedObject<T>> attach(const std::string& name, T* raw_ptr)
    {
        return std::make_shared<SharedObject<T>>(PrivateAttach(), name, raw_ptr);
    } 

protected:
    ShmMemSystemControl* shm_mem_sys_control_;

private:
    std::string name_;
    T* obj_ptr_ = nullptr;
};

template<typename T>
using SharedObjectPtr = std::shared_ptr<SharedObject<T>>;

class ShmObjManager
{
    struct ObjectInfo
    {
        std::any obj_ptr = nullptr;
        ObjectInfo() = default;
        ~ObjectInfo() = default;
    };
public:
    ~ShmObjManager();
    ShmObjManager(const ShmObjManager&) = delete;
    ShmObjManager& operator = (const ShmObjManager&) = delete;
    static ShmObjManager& instance();

    uint64_t getSharedObjMapUpdateTime();

    bool hasSharedObject(const std::string& name);

    void printObjName();

private:
    ShmObjManager();

    const pid_t self_pid_;
    FileLock key_file_lock_;
    ShmMemSystemControl* shm_mem_sys_control_;

    std::map<std::string, ObjectInfo> shm_obj_map_;

public:
    template<typename T, typename... ArgsT>
    SharedObjectPtr<T> create(const std::string& name, ArgsT&&... args);

    template<typename T>
    bool destroy(const std::string& name);
};

template <typename T, typename... ArgsT>
inline SharedObjectPtr<T> ShmObjManager::create(const std::string &name, ArgsT&&... args)
{
    SharedObjectPtr<T> ret = nullptr;

    auto in_it = shm_obj_map_.find(name);
    if(in_it != shm_obj_map_.end())
    {
        ret = std::any_cast<SharedObjectPtr<T>>(in_it->second.obj_ptr);
    }
    else
    {
        ProcessLockGuard<ProcessMutex> lock(shm_mem_sys_control_->mutex);

        // 检查是否已经存在，如果已经存在则直接挂接，如果不存在则创建
        auto it = shm_mem_sys_control_->shm_obj_map_->find( __inner::ShmObjString(name) );
        if(it != shm_mem_sys_control_->shm_obj_map_->end())
        {
            if(it->second.hash_code == typeid(T).hash_code())
            {
                it->second.owener_list.push_back(self_pid_);
                ret = SharedObject<T>::attach(name, (T*) it->second.obj_ptr );
            }
        }
        else
        {
            ret = SharedObject<T>::create(name, std::forward<ArgsT>(args)...);
            ShmObjInfo obj_info;
            obj_info.obj_ptr = ret->get();
            obj_info.owener_list.push_back(self_pid_);
            obj_info.obj_type = coin::type_name<T>();
            obj_info.hash_code = typeid(T).hash_code();
            shm_mem_sys_control_->shm_obj_map_->emplace(name, obj_info);
            shm_mem_sys_control_->setUpdateTime();
        }

        if(ret)
        {
            ret->shm_mem_sys_control_ = shm_mem_sys_control_;
            ObjectInfo oi;
            oi.obj_ptr = ret;
            shm_obj_map_.emplace(name, oi);
        }

    }

    return ret;
}

template <typename T>
inline bool ShmObjManager::destroy(const std::string &name)
{
    // 检查是否已经存在，如果已经存在则直接挂接，如果不存在则创建
    auto it = shm_obj_map_.find(name);
    if(it != shm_obj_map_.end())
    {
        shm_obj_map_.erase(it);
    }

    return true;
}

} // namespace coin::data
