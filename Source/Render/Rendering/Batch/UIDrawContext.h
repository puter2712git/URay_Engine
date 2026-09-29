#pragma once

#include "Render/Vertex.h"

#include "Core/Math/Color.h"
#include "Core/Math/Rect.h"
#include "Core/Type/Types.h"

#include <vector>

namespace URay::Render
{

struct UIDrawBatch
{
    uint32 indexOffset = 0;
    uint32 indexCount = 0;
    Rect clipRect = {};
};

class UIDrawContext
{
public:
    UIDrawContext();
    ~UIDrawContext();

public:
    void Clear();

    void PushClipRect(const Rect& rect);
    void PopClipRect();

    void AddFilledRect(const Rect& rect, const Color& color);

    const std::vector<UIDrawBatch>& GetBatches() const { return batches; }
    const std::vector<Render::VertexUI>& GetVertices() const { return vertices; }
    const std::vector<uint32>& GetIndices() const { return indices; }

private:
    Rect Intersect(const Rect& rectA, const Rect& rectB) const;

private:
    std::vector<Rect> clipStack;
    std::vector<UIDrawBatch> batches;

    std::vector<Render::VertexUI> vertices;
    std::vector<uint32> indices;
};

} // namespace URay::Render
