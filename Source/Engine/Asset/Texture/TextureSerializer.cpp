#include "TextureSerializer.h"

#include <Core/Binary/BinaryReader.h>
#include <Core/Binary/BinaryWriter.h>
#include <Core/File/VirtualFileSystem.h>
#include <Core/File/VirtualPath.h>
#include <Engine/Asset/Texture/TextureData.h>

namespace URay
{

constexpr uint32 TextureAssetMagic = 0x55525458; // "URTX"
constexpr uint32 TextureAssetVersion = 1;

TextureSerializer::TextureSerializer(VirtualFileSystem& fileSystem)
    : fileSystem(fileSystem) {}

bool TextureSerializer::Save(const TextureAssetData& texture, const VirtualPath& assetPath) const
{
    BinaryWriter writer;

    writer << TextureAssetMagic;
    writer << TextureAssetVersion;

    if (!texture.Serialize(writer))
        return false;

    const auto& buffer = writer.GetBuffer();

    return fileSystem.WriteBinary(assetPath, buffer);
}

bool TextureSerializer::Load(TextureAssetData& outTexture, const VirtualPath& assetPath) const
{
    auto buffer = fileSystem.ReadBinary(assetPath);
    if (buffer.empty())
        return false;

    BinaryReader reader(buffer);

    uint32 magic = 0;
    reader >> magic;

    if (magic != TextureAssetMagic)
        return false;

    uint32 version = 0;
    reader >> version;

    if (version != TextureAssetVersion)
        return false;

    return outTexture.Deserialize(reader);
}

} // namespace URay
