#include "impl/ipcs.hpp"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "buddy.hpp"

struct __shm_create_info_t {
    char key[512];
    void *addr;
    size_t size;
};

namespace coin::data
{
Shm::Shm(const std::string& key, void* addr, size_t size)
  : shm_(std::make_shared<coin::ipc::Shm>(key, addr, size))
{
    struct __shm_create_info_t info;
    memset(&info, 0, sizeof(struct __shm_create_info_t));

    // check whether shared memory exists, 
    // if exists and not mounted, destroy it
    if(shm_->num_of_attach() == 0)
    {
        shm_->destroy();
    }

    // create shared memory
    shm_->create();
    shm_->attach();

    // if create memory is not match to required memory
    // destroy shared memory and throw runtime exception
    if(shm_->addr() != addr)
    {
        fprintf(stderr, "create memory is not match(%p:%p).\n", shm_->addr(), addr);
        shm_->destroy();
        throw std::runtime_error("create memory is not match");
    }

    // buddy memory check
    // buddy_memory_check(*addr, size);
}
Shm::~Shm()
{
    if(shm_->is_finnally())
    {
        shm_->destroy();
    }
    return;
}

void* Shm::addr()
{
    if(shm_)
    {
        return shm_->addr();
    }
    return nullptr;
}
size_t Shm::size()
{
    if(shm_)
    {
        return shm_->size();
    }
    return 0;
}
} // namespace coin::data
