#pragma once

#include "Render/RenderInfo.h"
#include "Render/Vertex.h"

#include <vector>

namespace URay
{

class VertexBuffer;
class IndexBuffer;

class LineBatcher
{
public:
    void Reset();

private:
    std::array<VertexBuffer*, MAX_FRAMES_IN_FLIGHT> vertexBuffers;
    std::array<IndexBuffer*, MAX_FRAMES_IN_FLIGHT> indexBuffes;

    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
};

} // namespace URay
