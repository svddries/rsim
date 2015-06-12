#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>

namespace ipc = boost::interprocess;

//Typedefs of allocators and containers
typedef ipc::managed_shared_memory::segment_manager                       segment_manager_t;
typedef ipc::allocator<void, segment_manager_t>                           void_allocator;

namespace io
{

typedef ipc::basic_string<char, std::char_traits<char>, ipc::allocator<char, segment_manager_t> > string;

template <typename T>
using vector = ipc::vector<T, ipc::allocator<T, segment_manager_t> >;


}

// ----------------------------------------------------------------------------------------------------

class ComplexData
{

public:

    ComplexData(int id, const char *name, const void_allocator &void_alloc)
        : id_(id), char_string_(name, void_alloc), v(void_alloc)
    {}

    int id_;
    io::string char_string_;
    io::vector<int> v;

};

// ----------------------------------------------------------------------------------------------------

int main ()
{
   //Remove shared memory on construction and destruction
   struct shm_remove
   {
      shm_remove() { ipc::shared_memory_object::remove("MySharedMemory"); }
      ~shm_remove(){ ipc::shared_memory_object::remove("MySharedMemory"); }
   } remover;

   {
       //Create shared memory
       ipc::managed_shared_memory segment(ipc::create_only,"MySharedMemory", 65536);

       //An allocator convertible to any allocator<T, segment_manager_t> type
       void_allocator alloc_inst (segment.get_segment_manager());

       //Construct the shared memory map and fill it
       ComplexData* data = segment.construct<ComplexData>("MyData")(5, "test", alloc_inst);

       for(int i = 0; i < 2000; ++i)
       {
           data->v.push_back(i);
           std::cout << i << ": " << &data->v[0] << std::endl;
       }

       std::cout << &data->v[0] << std::endl;
   }

   {
       //Open the managed segment
       ipc::managed_shared_memory segment(ipc::open_only, "MySharedMemory");

       //Find the vector using the c-string name
       ComplexData* data = segment.find<ComplexData>("MyData").first;

       std::cout << &data->v[0] << std::endl;

//       for(unsigned int i = 0; i < data->v.size(); ++i)
//           std::cout << data->v[i] << std::endl;

       //When done, destroy the vector from the segment
       segment.destroy<ComplexData>("MyData");
   }

   return 0;
}
