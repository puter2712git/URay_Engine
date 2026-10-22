#include "TextureData.h"

#include <Core/Binary/BinaryReader.h>
#include <Core/Binary/BinaryWriter.h>

namespace URay
{

bool TextureAssetData::Serialize(BinaryWriter& writer) const
{
    writer << width;
    writer << height;
    writer << static_cast<uint32>(format);

    const uint64 pixelDataSize = static_cast<uint64>(pixels.size());
    writer << pixelDataSize;
    writer.WriteBytes(std::span<const uint8>(pixels));

    return true;
}

bool TextureAssetData::Deserialize(BinaryReader& reader)
{
    reader >> width;
    reader >> height;

    uint32 formatValue = 0;
    reader >> formatValue;
    format = static_cast<TexturePixelFormat>(formatValue);

    uint64 pixelDataSize = 0;
    reader >> pixelDataSize;

    pixels.resize(static_cast<size_t>(pixelDataSize));
    reader.ReadBytes(std::span<uint8>(pixels));

    return true;
}

} // namespace URay
