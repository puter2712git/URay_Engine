#pragma once

#include <string>

namespace URay
{

class VirtualPath
{
public:
    VirtualPath(const char* path);
    VirtualPath(const std::string& path);

public:
    const std::string& ToString() const { return path; }

private:
    std::string path;
};

} // namespace URay
