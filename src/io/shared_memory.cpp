#include "shared_memory.h"

#include "package.h"

namespace ipc = boost::interprocess;

#include <iostream>

namespace io
{

// ----------------------------------------------------------------------------------------------------

SharedMemory::SharedMemory()
{
}

// ----------------------------------------------------------------------------------------------------

SharedMemory::~SharedMemory()
{
//    if (!shared_mem_name_.empty())
//        ipc::shared_memory_object::remove(shared_mem_name_.c_str());
}

// ----------------------------------------------------------------------------------------------------

void SharedMemory::initialize(const char* name, Package& pkg)
{
    shared_mem_name_ = name;

    // Create a shared memory object
    shm = ipc::shared_memory_object(ipc::open_or_create, shared_mem_name_.c_str(), ipc::read_write);

    // Set size
    shm.truncate(pkg.size());

    // Map buffer region
    mem_ = ipc::mapped_region(shm, ipc::read_write, 0, pkg.size());

    pkg.mapTo(mem_.get_address());
}

} // end namespace io

