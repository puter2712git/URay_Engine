#pragma once

#include "VirtualPath.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace URay
{

namespace fs = std::filesystem;

struct VirtualFileEntry
{
    VirtualPath path;
    bool isDirectory = false;
};

class VirtualFilesystem
{
public:
    void Mount(const std::string& mountName, const fs::path& physicalPath);

    bool Exists(const VirtualPath& virtualPath) const;

    std::vector<uint8_t> ReadBinary(const VirtualPath& virtualPath) const;
    std::string ReadText(const VirtualPath& path) const;

    bool WriteText(const VirtualPath& path, const std::string& text) const;

    std::vector<VirtualFileEntry> ListDirectory(const VirtualPath& directory) const;

    fs::path ResolveToPhysicalPath(const VirtualPath& virtualPath) const;

private:
    std::unordered_map<std::string, fs::path> mountMap;
};

} // namespace URay
