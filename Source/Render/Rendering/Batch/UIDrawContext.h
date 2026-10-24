#pragma once

#include <Render/RHI/Vertex/Vertex.h>

#include <Engine/Asset/AssetHandle.h>

#include <Core/Math/Color.h>
#include <Core/Math/Rect.h>
#include <Core/Math/Vector2.h>
#include <Core/Type/Types.h>

#include <string_view>
#include <vector>

namespace URay::Render
{

class FontAtlas;
class TextureView;
class Sampler;

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

    TextureView* view = nullptr;
    Sampler* sampler = nullptr;
};

struct UIImage
{
    TextureView* view = nullptr;
    Sampler* sampler = nullptr;
    Vector2 uvMin = Vector2::Zero;
    Vector2 uvMax = Vector2::One;
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
    void AddOutlineRect(const Rect& rect, float thickness, const Color& color);
    void AddText(const Vector2& baselinePosition, std::string_view utf8Text, const TextStyle& style);
    void AddImage(const Rect& rect, const UIImage& image, const Color& color = Color::White);

    const std::vector<UIDrawBatch>& GetBatches() const { return batches; }
    const std::vector<Render::VertexUI>& GetVertices() const { return vertices; }
    const std::vector<uint32>& GetIndices() const { return indices; }

private:
    void AddQuad(const Rect& rect, const Vector2& uvMin, const Vector2& uvMax, const Color& color, TextureView* view, Sampler* sampler);

    Rect Intersect(const Rect& rectA, const Rect& rectB) const;

private:
    std::vector<Rect> clipStack;
    std::vector<UIDrawBatch> batches;

    std::vector<Render::VertexUI> vertices;
    std::vector<uint32> indices;
};

} // namespace URay::Render
