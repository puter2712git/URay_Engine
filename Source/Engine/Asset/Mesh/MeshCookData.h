#pragma once

#include "Engine/Asset/Mesh/Mesh.h"

#include "Core/Type/Types.h"
#include "Core/File/VirtualPath.h"
#include "Core/UUID.h"
#include "Core/UUID.h"

#include "Render/Vertex.h"

#include <vector>

namespace URay
{

struct MeshMaterialReference
{
    UUID uuid = {};
    VirtualPath sourcePath;
};

struct MeshCookData
{
    std::vector<Render::VertexPNT> vertices;
    std::vector<uint32> indices;
    std::vector<MeshSection> sections;
    std::vector<MeshMaterialReference> materials;
};

} // namespace URay
