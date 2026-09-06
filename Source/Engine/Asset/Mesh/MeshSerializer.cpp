#include "MeshSerializer.h"

#include <bit>
#include <utility>

namespace URay
{

namespace
{
constexpr uint32 MeshCookMagic = 0x4853454D; // MESH
constexpr uint32 MeshCookVersion = 1;
}

std::vector<uint8> MeshSerializer::Serialize(
    const MeshCookData& data) const
{
    std::vector<uint8> bytes;

    WriteBytes(bytes, MeshCookMagic);
    WriteBytes(bytes, MeshCookVersion);
    WriteBytes(bytes, static_cast<uint32>(data.vertices.size()));
    WriteBytes(bytes, static_cast<uint32>(data.indices.size()));
    WriteBytes(bytes, static_cast<uint32>(data.sections.size()));
    WriteBytes(bytes, static_cast<uint32>(data.materials.size()));

    for (const Render::VertexPNT& v : data.vertices)
    {
        WriteFloat(bytes, v.position.x);
        WriteFloat(bytes, v.position.y);
        WriteFloat(bytes, v.position.z);
        WriteFloat(bytes, v.normal.x);
        WriteFloat(bytes, v.normal.y);
        WriteFloat(bytes, v.normal.z);
        WriteFloat(bytes, v.uv.x);
        WriteFloat(bytes, v.uv.y);
    }

    for (uint32 index : data.indices)
    {
        WriteBytes(bytes, index);
    }

    for (const MeshSection& section : data.sections)
    {
        WriteBytes(bytes, section.indexOffset);
        WriteBytes(bytes, section.indexCount);
        WriteBytes(bytes, static_cast<uint32>(section.materialIndex));
    }

    for (const MeshMaterialReference& material : data.materials)
    {
        WriteUInt64(bytes, material.uuid.high);
        WriteUInt64(bytes, material.uuid.low);
        WriteString(bytes, material.sourcePath.ToString());
    }

    return bytes;
}

bool MeshSerializer::Deserialize(
    const std::vector<uint8>& bytes,
    MeshCookData& data) const
{
    constexpr size_t HeaderSize = sizeof(uint32) * 6;
    constexpr size_t VertexSize = sizeof(float) * 8;
    constexpr size_t IndexSize = sizeof(uint32);
    constexpr size_t SectionSize = sizeof(uint32) * 3;

    data = {};

    if (bytes.size() < HeaderSize)
        return false;

    size_t offset = 0;

    if (ReadBytes(bytes, offset) != MeshCookMagic)
        return false;
    offset += sizeof(uint32);

    if (ReadBytes(bytes, offset) != MeshCookVersion)
        return false;
    offset += sizeof(uint32);

    const uint32 vertexCount = ReadBytes(bytes, offset);
    offset += sizeof(uint32);

    const uint32 indexCount = ReadBytes(bytes, offset);
    offset += sizeof(uint32);

    const uint32 sectionCount = ReadBytes(bytes, offset);
    offset += sizeof(uint32);

    const uint32 materialCount = ReadBytes(bytes, offset);
    offset += sizeof(uint32);

    if (static_cast<size_t>(vertexCount) > (bytes.size() - offset) / VertexSize)
        return false;

    data.vertices.resize(vertexCount);
    for (Render::VertexPNT& vertex : data.vertices)
    {
        vertex.position.x = ReadFloat(bytes, offset);
        offset += sizeof(float);

        vertex.position.y = ReadFloat(bytes, offset);
        offset += sizeof(float);

        vertex.position.z = ReadFloat(bytes, offset);
        offset += sizeof(float);

        vertex.normal.x = ReadFloat(bytes, offset);
        offset += sizeof(float);

        vertex.normal.y = ReadFloat(bytes, offset);
        offset += sizeof(float);

        vertex.normal.z = ReadFloat(bytes, offset);
        offset += sizeof(float);

        vertex.uv.x = ReadFloat(bytes, offset);
        offset += sizeof(float);

        vertex.uv.y = ReadFloat(bytes, offset);
        offset += sizeof(float);
    }

    if (static_cast<size_t>(indexCount) > (bytes.size() - offset) / IndexSize)
        return false;

    data.indices.resize(indexCount);
    for (uint32& index : data.indices)
    {
        index = ReadBytes(bytes, offset);
        offset += sizeof(uint32);

        if (index >= vertexCount)
            return false;
    }

    if (static_cast<size_t>(sectionCount) > (bytes.size() - offset) / SectionSize)
        return false;

    data.sections.resize(sectionCount);
    for (MeshSection& section : data.sections)
    {
        section.indexOffset = ReadBytes(bytes, offset);
        offset += sizeof(uint32);

        section.indexCount = ReadBytes(bytes, offset);
        offset += sizeof(uint32);

        section.materialIndex = ReadBytes(bytes, offset);
        offset += sizeof(uint32);

        if (section.indexOffset > indexCount ||
            section.indexCount > indexCount - section.indexOffset)
        {
            return false;
        }
    }

    data.materials.reserve(materialCount);
    for (uint32 i = 0; i < materialCount; ++i)
    {
        constexpr size_t ReferenceSize = sizeof(uint64) * 2 + sizeof(uint32);
        if (bytes.size() - offset < ReferenceSize)
            return false;

        MeshMaterialReference material = {};
        material.uuid.high = ReadUInt64(bytes, offset);
        offset += sizeof(uint64);
        material.uuid.low = ReadUInt64(bytes, offset);
        offset += sizeof(uint64);

        const uint32 pathLength = ReadBytes(bytes, offset);
        offset += sizeof(uint32);
        if (pathLength > bytes.size() - offset)
            return false;

        material.sourcePath = VirtualPath(std::string(
            bytes.begin() + offset,
            bytes.begin() + offset + pathLength));
        offset += pathLength;
        data.materials.push_back(std::move(material));
    }

    return true;
}

void MeshSerializer::WriteBytes(
    std::vector<uint8>& bytes,
    uint32 value) const
{
    for (int32 shift = 0; shift < 32; shift += 8)
    {
        bytes.push_back(static_cast<uint8>(value >> shift));
    }
}

uint32 MeshSerializer::ReadBytes(
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

void MeshSerializer::WriteUInt64(
    std::vector<uint8>& bytes,
    uint64 value) const
{
    WriteBytes(bytes, static_cast<uint32>(value));
    WriteBytes(bytes, static_cast<uint32>(value >> 32));
}

uint64 MeshSerializer::ReadUInt64(
    const std::vector<uint8>& bytes,
    size_t offset) const
{
    const uint64 low = ReadBytes(bytes, offset);
    const uint64 high = ReadBytes(bytes, offset + sizeof(uint32));
    return low | (high << 32);
}

void MeshSerializer::WriteString(
    std::vector<uint8>& bytes,
    const std::string& value) const
{
    WriteBytes(bytes, static_cast<uint32>(value.size()));
    bytes.insert(bytes.end(), value.begin(), value.end());
}

void MeshSerializer::WriteFloat(
    std::vector<uint8>& bytes,
    float value) const
{
    WriteBytes(bytes, std::bit_cast<uint32>(value));
}

float MeshSerializer::ReadFloat(
    const std::vector<uint8>& bytes,
    size_t offset) const
{
    return std::bit_cast<float>(ReadBytes(bytes, offset));
}

} // namespace URay
