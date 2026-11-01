#pragma once

#include <Core/File/VirtualPath.h>
#include <Core/Type/Type.h>
#include <Core/Type/TypeAlias.h>

#include <filesystem>

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

    Vector<uint8> ReadBinary(const VirtualPath& virtualPath) const;
    String ReadText(const VirtualPath& path) const;

    bool WriteBinary(const VirtualPath& path, const Vector<uint8>& bin) const;
    bool WriteText(const VirtualPath& path, const String& text) const;

    Vector<VirtualFileEntry> ListDirectory(const VirtualPath& directory) const;

    std::filesystem::path ResolveToPhysicalPath(const VirtualPath& virtualPath) const;

private:
    HashMap<String, std::filesystem::path> mountMap;
};

} // namespace URay
