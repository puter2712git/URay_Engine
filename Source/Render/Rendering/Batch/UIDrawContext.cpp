#include "UIDrawContext.h"

#include "Render/RenderSystem.h"
#include "Render/Rendering/Font/FontSystem.h"
#include "Render/ResourceManager.h"

#include "Engine/Engine.h"

#include "Core/Type/String.h"

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
    AddQuad(rect, Vector2::Zero, Vector2::Zero, color, nullptr);
}

void UIDrawContext::AddText(const Vector2& position, std::string_view utf8Text, const TextStyle& style)
{
    RenderSystem& renderSystem = gEngine->GetRenderSystem();
    ResourceManager& resourceManager = renderSystem.GetResourceManager();
    FontSystem& fontSystem = resourceManager.GetFontSystem();

    FontAtlas* fontAtlas = fontSystem.GetOrCreateAtlas(style.fontHandle, style.pixelHeight);
    if (!fontAtlas)
        return;

    float penX = position.x;
    const float baselineY = position.y + fontAtlas->GetPixelHeight();

    std::u32string u32String = UTF8::Decode(utf8Text);

    for (char32_t codepoint : u32String)
    {
        const Glyph* glyph = fontSystem.GetOrCreateGlyph(fontAtlas, codepoint);
        if (!glyph)
            continue;

        if (glyph->atlasWidth > 0 && glyph->atlasHeight > 0)
        {
            Rect glyphRect = {};
            glyphRect.position = Vector2(penX + glyph->bearingX, baselineY - glyph->bearingY);
            glyphRect.size = Vector2(glyph->atlasWidth, glyph->atlasHeight);

            const Vector2 uvMin = Vector2(
                static_cast<float>(glyph->atlasX) / fontAtlas->GetWidth(),
                static_cast<float>(glyph->atlasY) / fontAtlas->GetHeight());
            const Vector2 uvMax = Vector2(
                static_cast<float>(glyph->atlasX + glyph->atlasWidth) / fontAtlas->GetWidth(),
                static_cast<float>(glyph->atlasY + glyph->atlasHeight) / fontAtlas->GetHeight());

            AddQuad(glyphRect, uvMin, uvMax, style.color, fontAtlas);
        }

        penX += glyph->advance;
    }
}

void UIDrawContext::AddQuad(const Rect& rect, const Vector2& uvMin, const Vector2& uvMax, const Color& color, FontAtlas* fontAtlas)
{
    if (rect.size.x <= 0.0f || rect.size.y <= 0.0f)
        return;

    // TODO: 에러 처리 설계 필요 (Debug/Release 빌드에서의 ASSERT, FATAL_ERROR 등)
    assert(!clipStack.empty());
    if (clipStack.empty())
        return;

    const Rect& clipRect = clipStack.back();
    if (clipRect.size.x <= 0.0f || clipRect.size.y <= 0.0f)
        return;

    const uint32 baseVertex = static_cast<uint32>(vertices.size());
    const uint32 indexOffset = static_cast<uint32>(indices.size());

    const Vector2 p0 = rect.position;
    const Vector2 p1 = Vector2(rect.position.x + rect.size.x, rect.position.y);
    const Vector2 p2 = Vector2(rect.position.x + rect.size.x, rect.position.y + rect.size.y);
    const Vector2 p3 = Vector2(rect.position.x, rect.position.y + rect.size.y);

    const Vector2 uv0 = uvMin;
    const Vector2 uv1 = Vector2(uvMax.x, uvMin.y);
    const Vector2 uv2 = uvMax;
    const Vector2 uv3 = Vector2(uvMin.x, uvMax.y);

    vertices.push_back(Render::VertexUI{ .position = p0, .uv = uv0, .color = color });
    vertices.push_back(Render::VertexUI{ .position = p1, .uv = uv1, .color = color });
    vertices.push_back(Render::VertexUI{ .position = p2, .uv = uv2, .color = color });
    vertices.push_back(Render::VertexUI{ .position = p3, .uv = uv3, .color = color });

    indices.insert(indices.end(),
                   { baseVertex + 0, baseVertex + 1, baseVertex + 2,
                     baseVertex + 0, baseVertex + 2, baseVertex + 3 });

    if (!batches.empty())
    {
        UIDrawBatch& lastBatch = batches.back();

        const bool isSameClip =
            lastBatch.clipRect.position.x == clipRect.position.x &&
            lastBatch.clipRect.position.y == clipRect.position.y &&
            lastBatch.clipRect.size.x == clipRect.size.x &&
            lastBatch.clipRect.size.y == clipRect.size.y;

        const bool isSameAtlas = lastBatch.fontAtlas == fontAtlas;

        const bool isContiguous = lastBatch.indexOffset + lastBatch.indexCount == indexOffset;

        if (isSameClip && isSameAtlas && isContiguous)
        {
            lastBatch.indexCount += 6;
            return;
        }
    }

    batches.push_back(UIDrawBatch{
        .indexOffset = indexOffset,
        .indexCount = 6,
        .clipRect = clipRect,
        .fontAtlas = fontAtlas });
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
