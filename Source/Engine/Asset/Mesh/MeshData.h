#pragma once

#include <Engine/Asset/AssetHandle.h>
#include <Engine/Asset/Mesh/MeshSection.h>
#include <Render/RHI/Vertex/Vertex.h>

#include <vector>

namespace URay
{

class BinaryReader;
class BinaryWriter;

struct MeshData
{
    std::vector<Render::VertexPNT> vertices;
    std::vector<uint32> indices;
    std::vector<MeshSection> sections;
    std::vector<AssetHandle> defaultMaterials;

    bool Serialize(BinaryWriter& writer) const;
    bool Deserialize(BinaryReader& reader);
    bool Validate() const;
};

} // namespace URay
