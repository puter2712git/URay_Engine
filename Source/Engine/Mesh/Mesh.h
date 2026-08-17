#pragma once

#include "Engine/Object/Object.h"

#include "Render/Vertex.h"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace URay
{

struct MeshSection
{
    uint32_t indexOffset = 0;
    uint32_t indexCount = 0;
    size_t materialIndex = 0;
};

class Mesh : public Object
{
    URAY_CLASS(Mesh, Object)

public:
    Mesh(const std::string& name,
         const std::vector<RHI::VertexPNT>& vertices, const std::vector<uint32_t>& indices);

public:
    const std::string& GetName() const { return name; }
    void SetName(const std::string& inName) { name = inName; }

    const std::vector<RHI::VertexPNT>& GetVertices() const { return vertices; }
    void SetVertices(const std::vector<RHI::VertexPNT>& newVertices) { vertices = newVertices; }

    const std::vector<uint32_t>& GetIndices() const { return indices; }
    void SetIndices(const std::vector<uint32_t>& newIndices) { indices = newIndices; }

private:
    std::string name;

    std::vector<RHI::VertexPNT> vertices;
    std::vector<uint32_t> indices;

    std::vector<MeshSection> sections;
};

} // namespace URay
