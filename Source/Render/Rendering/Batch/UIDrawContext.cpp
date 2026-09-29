#include "UIDrawContext.h"

#include <algorithm>
#include <cassert>

namespace URay::Render
{

UIDrawContext::UIDrawContext() = default;

UIDrawContext::~UIDrawContext() = default;

void UIDrawContext::Clear()
{
    clipStack.clear();
    batches.clear();
    vertices.clear();
    indices.clear();
}

void UIDrawContext::PushClipRect(const Rect& rect)
{
    if (clipStack.empty())
    {
        clipStack.push_back(rect);
        return;
    }

    const Rect& parentClip = clipStack.back();
    clipStack.push_back(Intersect(parentClip, rect));
}

void UIDrawContext::PopClipRect()
{
    assert(!clipStack.empty());
    clipStack.pop_back();
}

void UIDrawContext::AddFilledRect(const Rect& rect, const Color& color)
{
    if (rect.size.x <= 0.0f || rect.size.y <= 0.0f)
        return;

    const Rect& clipRect = clipStack.back();
    if (clipRect.size.x <= 0.0f || clipRect.size.y <= 0.0f)
        return;

    const uint32 baseIndex = static_cast<uint32>(vertices.size());
    const uint32 indexOffset = static_cast<uint32>(indices.size());

    const Vector2 p0 = Vector2(rect.position.x, rect.position.y);
    const Vector2 p1 = Vector2(rect.position.x + rect.size.x, rect.position.y);
    const Vector2 p2 = Vector2(rect.position.x + rect.size.x, rect.position.y + rect.size.y);
    const Vector2 p3 = Vector2(rect.position.x, rect.position.y + rect.size.y);

    vertices.push_back(VertexUI{ .position = p0, .uv = {}, .color = color });
    vertices.push_back(VertexUI{ .position = p1, .uv = {}, .color = color });
    vertices.push_back(VertexUI{ .position = p2, .uv = {}, .color = color });
    vertices.push_back(VertexUI{ .position = p3, .uv = {}, .color = color });

    indices.insert(indices.end(),
                   { baseIndex + 0, baseIndex + 1, baseIndex + 2,
                     baseIndex + 0, baseIndex + 2, baseIndex + 3 });

    if (!batches.empty())
    {
        UIDrawBatch& lastBatch = batches.back();

        const bool isClipSame =
            lastBatch.clipRect.position.x == clipRect.position.x &&
            lastBatch.clipRect.position.y == clipRect.position.y &&
            lastBatch.clipRect.size.x == clipRect.size.x &&
            lastBatch.clipRect.size.y == clipRect.size.y;

        const bool isContiguous = lastBatch.indexOffset + lastBatch.indexCount == indexOffset;

        if (isClipSame && isContiguous)
        {
            lastBatch.indexCount += 6;
            return;
        }
    }

    batches.push_back(UIDrawBatch{
        .indexOffset = indexOffset,
        .indexCount = 6,
        .clipRect = clipRect });
}

Rect UIDrawContext::Intersect(const Rect& rectA, const Rect& rectB) const
{
    Rect ret = {};

    const float minX = std::max(rectA.position.x, rectB.position.x);
    const float minY = std::max(rectA.position.y, rectB.position.y);

    const float maxX = std::min(rectA.position.x + rectA.size.x, rectB.position.x + rectB.size.x);
    const float maxY = std::min(rectA.position.y + rectA.size.y, rectB.position.y + rectB.size.y);

    ret.position = Vector2(minX, minY);
    ret.size = Vector2(std::max(0.0f, maxX - minX), std::max(0.0f, maxY - minY));

    return ret;
}

} // namespace URay::Render
