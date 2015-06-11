#include "shared_memory.h"

#include "package.h"
#include "memory.h"

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

void SharedMemory::create(const char* name, Package& pkg)
{
    shared_mem_name_ = name;

    // Create a shared memory object
    shm = ipc::shared_memory_object(ipc::open_or_create, shared_mem_name_.c_str(), ipc::read_write);

    // Serialize the package specification (first to a buffer, because we do not know how big it is)
    io::GrowingBuffer spec;
    pkg.SerializeSpecification(spec);

    // Set size: specification size + package size
    shm.truncate(spec.size() + pkg.size());

    // Map the specification region
    mem_spec_ = ipc::mapped_region(shm, ipc::read_write, 0, spec.size());

    // Map the data (pkg) region
    mem_pkg_ = ipc::mapped_region(shm, ipc::read_write, spec.size(), pkg.size());

    // Copy the package specification to the shared memory
    spec.copyTo(mem_spec_.get_address());

    // Map the package to the data address
    pkg.mapTo(mem_pkg_.get_address());
}

// ----------------------------------------------------------------------------------------------------

void SharedMemory::open(const char* name, Package& pkg)
{
    shared_mem_name_ = name;

    // Create a shared memory object
    shm = ipc::shared_memory_object(ipc::open_only, shared_mem_name_.c_str(), ipc::read_write);

    // We don't know how big the package specification is
    ipc::mapped_region mem_tmp = ipc::mapped_region(shm, ipc::read_write, 0, 0);

    uint64_t spec_size = pkg.DeserializeSpecification(mem_tmp.get_address());

    // Map the specification region
    mem_spec_ = ipc::mapped_region(shm, ipc::read_only, 0, spec_size);

    // Map the data (pkg) region
    mem_pkg_ = ipc::mapped_region(shm, ipc::read_write, spec_size, pkg.size());

    // Map the package to the data address
    pkg.mapTo(mem_pkg_.get_address());
}

} // end namespace io

