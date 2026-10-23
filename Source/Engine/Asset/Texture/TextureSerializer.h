#pragma once

namespace URay
{

class VirtualFileSystem;
class VirtualPath;
struct TextureData;

class TextureSerializer
{
public:
    TextureSerializer(VirtualFileSystem& fileSystem);

public:
    bool Save(const TextureData& texture, const VirtualPath& assetPath) const;
    bool Load(TextureData& outTexture, const VirtualPath& assetPath) const;

private:
    VirtualFileSystem& fileSystem;
};

} // namespace URay
