#pragma once

#include "Render/Rendering/Font/FontAtlas.h"
#include "Render/Rendering/Font/FontFace.h"

#include "Engine/Asset/Asset.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <memory>
#include <unordered_map>

namespace URay
{
class Font;
}

namespace URay::Render
{

class FontSystem
{
public:
    FontSystem();
    ~FontSystem();

public:
    bool Initialize();
    void Finalize();

    FontFace* GetOrCreateFace(URay::Font* font);
    FontAtlas* GetOrCreateAtlas(FontFace* face, uint32 pixelHeight);
    const Glyph* GetOrCreateGlyph(FontAtlas* atlas, char32_t codepoint);

private:
    FT_Library library = nullptr;

    std::unordered_map<AssetHandle, std::unique_ptr<FontFace>, AssetHandleHash> faces;
    std::unordered_map<FontAtlasKey, std::unique_ptr<FontAtlas>, FontAtlasKeyHash> atlases;
};

} // namespace URay::Render
