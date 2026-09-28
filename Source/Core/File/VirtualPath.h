#pragma once

#include <string>

namespace URay
{

class VirtualPath
{
public:
    VirtualPath();
    VirtualPath(const char* path);
    VirtualPath(const std::string& path);

public:
    VirtualPath GetDirectory() const;
    std::string GetFilename() const;
    std::string GetStem() const;
    std::string GetExtension() const;

    VirtualPath RemoveExtension() const;
    VirtualPath ReplaceExtension(const std::string& extension) const;
    VirtualPath Join(const std::string& child) const;

    std::string GetMountName() const;
    std::string GetRelativePath() const;

    bool IsDirectory() const;

    const std::string& ToString() const { return path; }

private:
    std::string path;
};

} // namespace URay
