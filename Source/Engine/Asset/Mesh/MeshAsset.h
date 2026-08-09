#pragma once

#include "Render/Vertex.h"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace URay
{

class MeshAsset
{
public:
    const std::string& GetName() const { return name; }
    void SetName(const std::string& inName) { name = inName; }

    const std::vector<VertexPNT>& GetVertices() const { return vertices; }
    void SetVertices(const std::vector<VertexPNT>& newVertices) { vertices = newVertices; }

    const std::vector<uint32_t>& GetIndices() const { return indices; }
    void SetIndices(const std::vector<uint32_t>& newIndices) { indices = newIndices; }

private:
    std::string name;

    std::vector<VertexPNT> vertices;
    std::vector<uint32_t> indices;
};

} // namespace URay
