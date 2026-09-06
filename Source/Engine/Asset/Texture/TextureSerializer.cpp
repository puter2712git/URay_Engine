#include "TextureSerializer.h"

namespace URay
{

std::vector<uint8> TextureSerializer::Serialize(
    const TextureCookData& data) const
{
    std::vector<uint8> bytes;

    WriteBytes(bytes, static_cast<uint32>(data.width));
    WriteBytes(bytes, static_cast<uint32>(data.height));
    WriteBytes(bytes, static_cast<uint32>(data.channels));

    bytes.insert(bytes.end(), data.pixels.begin(), data.pixels.end());

    return bytes;
}

TextureCookData TextureSerializer::Deserialize(
    const std::vector<uint8>& bytes) const
{
    TextureCookData data = {};

    const uint32 width = ReadBytes(bytes, 0);
    const uint32 height = ReadBytes(bytes, 4);
    const uint32 channels = ReadBytes(bytes, 8);

    data.width = static_cast<int32>(width);
    data.height = static_cast<int32>(height);
    data.channels = static_cast<int32>(channels);
    data.pixels.assign(bytes.begin() + 12, bytes.end());

    return data;
}

void TextureSerializer::WriteBytes(
    std::vector<uint8>& bytes,
    uint32 value) const
{
    for (int32 shift = 0; shift < 32; shift += 8)
    {
        bytes.push_back(static_cast<uint8>(value >> shift));
    }
}

uint32 TextureSerializer::ReadBytes(
    const std::vector<uint8>& bytes,
    size_t offset) const
{
    uint32 value = 0;
    for (size_t i = 0; i < 4; ++i)
    {
        value |= static_cast<uint32>(bytes[offset + i]) << (i * 8);
    }
    return value;
}

} // namespace URay
