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

    const std::vector<Vertex>& GetVertices() const { return vertices; }
    void SetVertices(const std::vector<Vertex>& newVertices) { vertices = newVertices; }

    const std::vector<uint16_t>& GetIndices() const { return indices; }
    void SetIndices(const std::vector<uint16_t>& newIndices) { indices = newIndices; }

private:
    std::string name;

    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
};

} // namespace URay
