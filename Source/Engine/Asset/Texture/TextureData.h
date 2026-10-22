#pragma once

#include <Core/File/VirtualPath.h>
#include <Core/Type/Types.h>

#include <vector>

namespace URay
{

class BinaryReader;
class BinaryWriter;

enum class TexturePixelFormat
{
    RGBA8
};

struct TextureImportData
{
    VirtualPath sourcePath;

    uint32 importerVersion = 1;

    bool isSRGB = true;
    bool generateMipmaps = false;
};

struct TextureAssetData
{
    uint32 width = 0;
    uint32 height = 0;
    TexturePixelFormat format = TexturePixelFormat::RGBA8;
    std::vector<uint8> pixels;

    bool Serialize(BinaryWriter& writer) const;
    bool Deserialize(BinaryReader& reader);
};

} // namespace URay
