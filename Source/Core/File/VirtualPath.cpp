#include "VirtualPath.h"

namespace URay
{

VirtualPath::VirtualPath(const char* path)
    : path(path)
{
}

VirtualPath::VirtualPath(const std::string& path)
    : path(path)
{
}

} // namespace URay
