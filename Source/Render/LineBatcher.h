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
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
};

} // namespace URay
