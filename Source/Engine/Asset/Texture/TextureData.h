#pragma once

#include <Core/File/VirtualPath.h>
#include <Core/Type/TypeAlias.h>

#include <vector>

namespace URay
{

class BinaryReader;
class BinaryWriter;

enum class TexturePixelFormat
{
    RGBA8
};

struct TextureData
{
    uint32 width = 0;
    uint32 height = 0;
    TexturePixelFormat format = TexturePixelFormat::RGBA8;
    std::vector<uint8> pixels;

    bool Serialize(BinaryWriter& writer) const;
    bool Deserialize(BinaryReader& reader);
};

} // namespace URay
