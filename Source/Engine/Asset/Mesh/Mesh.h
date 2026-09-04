#pragma once

#include "Engine/Asset/Asset.h"

#include "Core/Math/AABB.h"
#include "Core/Type/Types.h"

#include "Render/Vertex.h"

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace URay
{

class Material;

struct MeshSection
{
    uint32 indexOffset = 0;
    uint32 indexCount = 0;
    size_t materialIndex = 0;
};

class Mesh : public Asset
{
    URAY_CLASS(Mesh, Asset)

public:
    Mesh(const std::string& name);

public:
    const std::string& GetName() const { return name; }
    void SetName(const std::string& inName) { name = inName; }

    const std::vector<Render::VertexPNT>& GetVertices() const { return vertices; }
    void SetVertices(const std::vector<Render::VertexPNT>& newVertices);

    const std::vector<uint32>& GetIndices() const { return indices; }
    void SetIndices(const std::vector<uint32>& newIndices) { indices = newIndices; }

    const std::vector<MeshSection>& GetSections() const { return sections; }
    void SetSections(const std::vector<MeshSection>& newSections) { sections = newSections; }

    Material* GetDefaultMaterial(size_t index) const { return index < defaultMaterials.size() ? defaultMaterials[index] : nullptr; }
    const std::vector<Material*>& GetDefaultMaterials() const { return defaultMaterials; }
    void SetDefaultMaterials(const std::vector<Material*> newMaterials) { defaultMaterials = newMaterials; }

    const AABB& GetLocalBounds() const { return localBounds; }

private:
    std::string name;
    std::vector<Render::VertexPNT> vertices;
    std::vector<uint32> indices;
    std::vector<MeshSection> sections;
    std::vector<Material*> defaultMaterials;

    AABB localBounds = {};
};

} // namespace URay
