#include "MeshData.h"

#include <Core/Binary/BinaryReader.h>
#include <Core/Binary/BinaryWriter.h>

namespace URay
{

bool MeshData::Serialize(BinaryWriter& writer) const
{
    writer << static_cast<uint32>(vertices.size());
    for (const auto& vertex : vertices)
    {
        writer << vertex.position.x;
        writer << vertex.position.y;
        writer << vertex.position.z;
        writer << vertex.normal.x;
        writer << vertex.normal.y;
        writer << vertex.normal.z;
        writer << vertex.uv.x;
        writer << vertex.uv.y;
    }

    writer << static_cast<uint32>(indices.size());
    for (const auto& index : indices)
    {
        writer << index;
    }

    writer << static_cast<uint32>(sections.size());
    for (const auto& section : sections)
    {
        writer << section.indexOffset;
        writer << section.indexCount;
        writer << section.materialIndex;
    }

    writer << static_cast<uint32>(defaultMaterials.size());
    for (const auto& material : defaultMaterials)
    {
        writer << material.high;
        writer << material.low;
    }

    return true;
}

bool MeshData::Deserialize(BinaryReader& reader)
{
    uint32 vertexCount = 0;
    reader >> vertexCount;

    vertices.resize(vertexCount);
    for (auto& vertex : vertices)
    {
        reader >> vertex.position.x;
        reader >> vertex.position.y;
        reader >> vertex.position.z;
        reader >> vertex.normal.x;
        reader >> vertex.normal.y;
        reader >> vertex.normal.z;
        reader >> vertex.uv.x;
        reader >> vertex.uv.y;
    }

    uint32 indexCount = 0;
    reader >> indexCount;

    indices.resize(indexCount);
    for (auto& index : indices)
    {
        reader >> index;
    }

    uint32 sectionCount = 0;
    reader >> sectionCount;

    sections.resize(sectionCount);
    for (auto& section : sections)
    {
        reader >> section.indexOffset;
        reader >> section.indexCount;
        reader >> section.materialIndex;
    }

    uint32 materialCount = 0;
    reader >> materialCount;

    defaultMaterials.resize(materialCount);
    for (auto& material : defaultMaterials)
    {
        reader >> material.high;
        reader >> material.low;
    }

    return true;
}

bool MeshData::Validate() const
{
    for (uint32 index : indices)
    {
        if (index >= vertices.size())
            return false;
    }

    for (const MeshSection& section : sections)
    {
        const uint64 sectionEnd = static_cast<uint64>(section.indexOffset) + section.indexCount;

        if (sectionEnd > indices.size())
            return false;

        if (section.indexCount % 3 != 0)
            return false;

        if (section.materialIndex >= defaultMaterials.size())
            return false;
    }

    return true;
}

} // namespace URay
