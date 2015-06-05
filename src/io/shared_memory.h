#ifndef RSIM_IO_SHARED_MEMORY_H_
#define RSIM_IO_SHARED_MEMORY_H_

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

namespace io
{

class Package;

class SharedMemory
{

public:

    SharedMemory();

    ~SharedMemory();

    void create(const char* name, Package& pkg);

    void open(const char* name, Package& pkg);

private:

    std::string shared_mem_name_;

    boost::interprocess::shared_memory_object shm;
    boost::interprocess::mapped_region mem_spec_;
    boost::interprocess::mapped_region mem_pkg_;

};

} // end namespace io

#endif
