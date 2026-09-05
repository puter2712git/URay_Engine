#pragma once

#include "Engine/Asset/Mesh/Mesh.h"
#include "Engine/Asset/Mesh/MeshGenerator.h"

#include "Render/Vertex.h"

#include "Core/Type/Types.h"

namespace URay
{

class Engine;
class Material;
class Texture;
struct AssetMetadata;

namespace Render
{
class Shader;
}

class AssetFactory
{
public:
    AssetFactory(Engine& engine);
    ~AssetFactory();

public:
    Mesh* CreateMesh(const AssetMetadata& metadata,
                     const std::vector<Render::VertexPNT>& vertices,
                     const std::vector<uint32>& indices,
                     const std::vector<MeshSection>& sections,
                     const std::vector<Material*>& materials);
    Material* CreateMaterial(const AssetMetadata& metadata, Render::Shader* shader);
    Texture* CreateTexture(const AssetMetadata& metadata,
                           int32 width, int32 height, int32 channels,
                           const std::vector<uint8>& pixels);

private:
    Engine& engine;
};

} // namespace URay
