#include "MaterialSerializer.h"

#include <bit>

namespace URay
{

namespace
{
constexpr uint32 MaterialCookMagic = 0x4C54414D; // MATL
constexpr uint32 MaterialCookVersion = 1;
}

std::vector<uint8> MaterialSerializer::Serialize(const MaterialCookData& data) const
{
    std::vector<uint8> bytes;
    WriteUInt32(bytes, MaterialCookMagic);
    WriteUInt32(bytes, MaterialCookVersion);
    WriteFloat(bytes, data.baseColor.r);
    WriteFloat(bytes, data.baseColor.g);
    WriteFloat(bytes, data.baseColor.b);
    WriteFloat(bytes, data.baseColor.a);
    WriteString(bytes, data.shaderName);
    WriteString(bytes, data.baseColorTexturePath.ToString());
    return bytes;
}

bool MaterialSerializer::Deserialize(
    const std::vector<uint8>& bytes,
    MaterialCookData& data) const
{
    constexpr size_t FixedSize = sizeof(uint32) * 2 + sizeof(float) * 4;
    if (bytes.size() < FixedSize ||
        ReadUInt32(bytes, 0) != MaterialCookMagic ||
        ReadUInt32(bytes, sizeof(uint32)) != MaterialCookVersion)
    {
        return false;
    }

    size_t offset = sizeof(uint32) * 2;
    data = {};
    data.baseColor.r = ReadFloat(bytes, offset); offset += sizeof(float);
    data.baseColor.g = ReadFloat(bytes, offset); offset += sizeof(float);
    data.baseColor.b = ReadFloat(bytes, offset); offset += sizeof(float);
    data.baseColor.a = ReadFloat(bytes, offset); offset += sizeof(float);

    const auto ReadString = [&bytes, &offset](std::string& value)
    {
        if (bytes.size() - offset < sizeof(uint32))
            return false;

        uint32 length = 0;
        for (size_t i = 0; i < sizeof(uint32); ++i)
            length |= static_cast<uint32>(bytes[offset + i]) << (i * 8);
        offset += sizeof(uint32);

        if (length > bytes.size() - offset)
            return false;

        value.assign(bytes.begin() + offset, bytes.begin() + offset + length);
        offset += length;
        return true;
    };

    std::string texturePath;
    if (!ReadString(data.shaderName) || !ReadString(texturePath))
        return false;

    data.baseColorTexturePath = VirtualPath(texturePath);
    return true;
}

void MaterialSerializer::WriteUInt32(std::vector<uint8>& bytes, uint32 value) const
{
    for (int32 shift = 0; shift < 32; shift += 8)
        bytes.push_back(static_cast<uint8>(value >> shift));
}

uint32 MaterialSerializer::ReadUInt32(const std::vector<uint8>& bytes, size_t offset) const
{
    uint32 value = 0;
    for (size_t i = 0; i < sizeof(uint32); ++i)
        value |= static_cast<uint32>(bytes[offset + i]) << (i * 8);
    return value;
}

void MaterialSerializer::WriteFloat(std::vector<uint8>& bytes, float value) const
{
    WriteUInt32(bytes, std::bit_cast<uint32>(value));
}

float MaterialSerializer::ReadFloat(const std::vector<uint8>& bytes, size_t offset) const
{
    return std::bit_cast<float>(ReadUInt32(bytes, offset));
}

void MaterialSerializer::WriteString(std::vector<uint8>& bytes, const std::string& value) const
{
    WriteUInt32(bytes, static_cast<uint32>(value.size()));
    bytes.insert(bytes.end(), value.begin(), value.end());
}

} // namespace URay
