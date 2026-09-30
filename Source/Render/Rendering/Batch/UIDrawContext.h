#pragma once

#include "Render/Vertex.h"

#include "Engine/Asset/Asset.h"

#include "Core/Math/Color.h"
#include "Core/Math/Rect.h"
#include "Core/Math/Vector2.h"
#include "Core/Type/Types.h"

#include <string_view>
#include <vector>

namespace URay::Render
{

class FontAtlas;

struct TextStyle
{
    AssetHandle fontHandle;
    uint32 pixelHeight = 18;
    Color color = Color::White;
};

struct UIDrawBatch
{
    uint32 indexOffset = 0;
    uint32 indexCount = 0;
    Rect clipRect = {};

    const FontAtlas* fontAtlas = nullptr;
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
    void AddText(const Vector2& position, std::string_view utf8Text, const TextStyle& style);

    const std::vector<UIDrawBatch>& GetBatches() const { return batches; }
    const std::vector<Render::VertexUI>& GetVertices() const { return vertices; }
    const std::vector<uint32>& GetIndices() const { return indices; }

private:
    void AddQuad(const Rect& rect, const Vector2& uvMin, const Vector2& uvMax, const Color& color, FontAtlas* fontAtlas);

    Rect Intersect(const Rect& rectA, const Rect& rectB) const;

private:
    std::vector<Rect> clipStack;
    std::vector<UIDrawBatch> batches;

    std::vector<Render::VertexUI> vertices;
    std::vector<uint32> indices;
};

} // namespace URay::Render
