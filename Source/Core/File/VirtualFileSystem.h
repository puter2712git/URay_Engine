#pragma once
#include <Core/Type/Type.h>
#include <Core/Type/TypeAlias.h>

#include <Core/File/VirtualPath.h>

#include <filesystem>
#include <unordered_map>
#include <vector>

namespace URay
{

struct VirtualFileEntry
{
    URAY_PLAIN_CLASS(VirtualFileEntry)

    VirtualPath path;
    bool isDirectory = false;
};

class VirtualFileSystem
{
public:
    bool Initialize(const String& enginePath, const String& projectPath);
    void Finalize();

public:
    void Mount(const String& mountName, const std::filesystem::path& physicalPath);

    bool Exists(const VirtualPath& path) const;
    bool IsDirectory(const VirtualPath& path) const;

    std::vector<uint8> ReadBinary(const VirtualPath& virtualPath) const;
    String ReadText(const VirtualPath& path) const;

    bool WriteBinary(const VirtualPath& path, const std::vector<uint8>& bin) const;
    bool WriteText(const VirtualPath& path, const String& text) const;

    std::vector<VirtualFileEntry> ListDirectory(const VirtualPath& directory) const;

    std::filesystem::path ResolveToPhysicalPath(const VirtualPath& virtualPath) const;

private:
    std::unordered_map<String, std::filesystem::path> mountMap;
};

} // namespace URay
