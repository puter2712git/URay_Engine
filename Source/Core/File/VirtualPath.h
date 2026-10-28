#pragma once
#include <Core/Type/TypeAlias.h>
#include <Core/Type/Type.h>


namespace URay
{

class VirtualPath
{
    URAY_TYPE(VirtualPath)

public:
    VirtualPath();
    VirtualPath(const char* path);
    VirtualPath(const String& path);

public:
    VirtualPath GetDirectory() const;
    String GetFilename() const;
    String GetStem() const;
    String GetExtension() const;

    VirtualPath RemoveExtension() const;
    VirtualPath ReplaceExtension(const String& extension) const;
    VirtualPath Join(const String& child) const;

    String GetMountName() const;
    String GetRelativePath() const;

    bool IsDirectory() const;

    const String& ToString() const { return path; }

private:
    String path;
};

} // namespace URay
