#pragma once

namespace URay
{

class VirtualFileSystem;
class VirtualPath;
struct FontData;

class FontSerializer
{
public:
    FontSerializer(VirtualFileSystem& fileSystem);

public:
    bool Save(const FontData& font, const VirtualPath& assetPath) const;
    bool Load(FontData& outFont, const VirtualPath& assetPath) const;

private:
    VirtualFileSystem& fileSystem;
};

} // namespace URay
