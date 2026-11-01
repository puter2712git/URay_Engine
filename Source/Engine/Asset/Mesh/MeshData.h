#pragma once

#include <Core/Type/TypeAlias.h>
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
    Vector<Render::VertexPNT> vertices;
    Vector<uint32> indices;
    Vector<MeshSection> sections;
    Vector<AssetHandle> defaultMaterials;

    bool Serialize(BinaryWriter& writer) const;
    bool Deserialize(BinaryReader& reader);
    bool Validate() const;
};

} // namespace URay
