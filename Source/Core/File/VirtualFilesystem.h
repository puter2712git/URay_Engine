#pragma once

#include "VirtualPath.h"

#include "Core/Type/Types.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace URay
{

struct VirtualFileEntry
{
    VirtualPath path;
    bool isDirectory = false;
};

class VirtualFileSystem
{
public:
    VirtualFileSystem();
    ~VirtualFileSystem();

public:
    bool Initialize(const std::string& enginePath, const std::string& projectPath);
    void Finalize();

public:
    void Mount(const std::string& mountName, const std::filesystem::path& physicalPath);

    bool Exists(const VirtualPath& path) const;
    bool IsDirectory(const VirtualPath& path) const;

    std::vector<uint8> ReadBinary(const VirtualPath& virtualPath) const;
    std::string ReadText(const VirtualPath& path) const;

    bool WriteBinary(const VirtualPath& path, const std::vector<uint8>& bin) const;
    bool WriteText(const VirtualPath& path, const std::string& text) const;

    std::vector<VirtualFileEntry> ListDirectory(const VirtualPath& directory) const;

    std::filesystem::path ResolveToPhysicalPath(const VirtualPath& virtualPath) const;

private:
    std::unordered_map<std::string, std::filesystem::path> mountMap;
};

} // namespace URay
