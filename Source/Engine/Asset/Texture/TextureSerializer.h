#pragma once

namespace URay
{

class VirtualFileSystem;
class VirtualPath;
struct TextureAssetData;

class TextureSerializer
{
public:
    TextureSerializer(VirtualFileSystem& fileSystem);

public:
    bool Save(const TextureAssetData& texture, const VirtualPath& assetPath) const;
    bool Load(TextureAssetData& outTexture, const VirtualPath& assetPath) const;

private:
    VirtualFileSystem& fileSystem;
};

} // namespace URay
