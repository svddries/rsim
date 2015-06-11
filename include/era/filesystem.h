#ifndef ERA_FILESYSTEM_H_
#define ERA_FILESYSTEM_H_

#include <string>

namespace era
{
namespace filesystem
{

// ----------------------------------------------------------------------------------------------------

std::string getParentPath(const std::string& path)
{
    std::size_t idx = path.find_last_of("/");
    if (idx == std::string::npos)
        return "./";
    return path.substr(0, idx) + "/";
}

} // end namespace era

} // end namespace filesystem


#endif
