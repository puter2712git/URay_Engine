#pragma once

#include "Render/RHI/Texture/Texture.h"

#include "Engine/Asset/Asset.h"

#include "Core/Math/Rect.h"
#include "Core/Math/Vector2.h"
#include "Core/Type/Types.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace URay::Render
{

class FontFace;
class Texture;
class TextureView;

struct Glyph
{
    uint32 atlasX = 0;
    uint32 atlasY = 0;
    uint32 atlasWidth = 0;
    uint32 atlasHeight = 0;

    int32 bearingX = 0;
    int32 bearingY = 0;
    float advance = 0.0f;
};

struct FontAtlasKey
{
    AssetHandle fontHandle;
    uint32 pixelHeight;

    bool operator==(const FontAtlasKey&) const = default;
};

struct FontAtlasKeyHash
{
    size_t operator()(const FontAtlasKey& key) const
    {
        const size_t fontHash = AssetHandleHash{}(key.fontHandle);
        const size_t sizeHash = std::hash<uint32>{}(key.pixelHeight);

        return fontHash ^ (sizeHash + 0x9e3779b9 + (fontHash << 6) + (fontHash >> 2));
    }
};

class FontAtlas
{
    friend class FontSystem;

public:
    FontAtlas(FontFace* face, uint32 pixelHeight);
    ~FontAtlas();

public:
    const Glyph* FindGlyph(char32_t codepoint) const;

    uint32 GetPixelHeight() const { return pixelHeight; }
    uint32 GetWidth() const { return width; }
    uint32 GetHeight() const { return height; }
    TextureView* GetTextureView() const { return view.get(); }

private:
    FontFace* face = nullptr;
    uint32 pixelHeight = 0;

    uint32 width = 1024;
    uint32 height = 1024;
    uint32 padding = 1;

    std::vector<uint8> pixels;
    std::unordered_map<char32_t, Glyph> glyphs;

    uint32 cursorX = 0;
    uint32 cursorY = 0;
    uint32 rowHeight = 0;

    bool isDirty = false;

    std::unique_ptr<Texture> texture;
    std::unique_ptr<TextureView> view;

    TextureRegion dirtyRegion = {};
};

} // namespace URay::Render
