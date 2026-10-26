#include "FontSerializer.h"

#include <Core/Binary/BinaryReader.h>
#include <Core/Binary/BinaryWriter.h>
#include <Core/File/VirtualFileSystem.h>
#include <Core/File/VirtualPath.h>
#include <Core/Type/TypeAlias.h>
#include <Engine/Asset/Font/FontData.h>

namespace URay
{

constexpr uint32 FontAssetMagic = 0x5552464E; // "URFN"
constexpr uint32 FontAssetVersion = 1;

FontSerializer::FontSerializer(VirtualFileSystem& fileSystem)
    : fileSystem(fileSystem) {}

bool FontSerializer::Save(const FontData& font, const VirtualPath& assetPath) const
{
    BinaryWriter writer;

    writer << FontAssetMagic;
    writer << FontAssetVersion;

    if (!font.Serialize(writer))
        return false;

    const auto& buffer = writer.GetBuffer();

    return fileSystem.WriteBinary(assetPath, buffer);
}

bool FontSerializer::Load(FontData& outFont, const VirtualPath& assetPath) const
{
    auto buffer = fileSystem.ReadBinary(assetPath);
    if (buffer.empty())
        return false;

    BinaryReader reader(buffer);

    uint32 magic = 0;
    reader >> magic;

    if (magic != FontAssetMagic)
        return false;

    uint32 version = 0;
    reader >> version;

    if (version != FontAssetVersion)
        return false;

    return outFont.Deserialize(reader);
}

} // namespace URay
