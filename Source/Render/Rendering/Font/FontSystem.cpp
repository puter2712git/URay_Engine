#include "FontSystem.h"

#include "Engine/Asset/Font/Font.h"

#include <algorithm>

namespace URay::Render
{

FontSystem::FontSystem() = default;

FontSystem::~FontSystem() = default;

bool FontSystem::Initialize()
{
    return FT_Init_FreeType(&library) == FT_Err_Ok;
}

void FontSystem::Finalize()
{
    atlases.clear();
    faces.clear();

    FT_Done_FreeType(library);
    library = nullptr;
}

FontFace* FontSystem::GetOrCreateFace(URay::Font* font)
{
    FontFace* ret = nullptr;

    const AssetHandle handle = font->GetHandle();

    const auto it = faces.find(handle);
    if (it != faces.end())
    {
        ret = it->second.get();
        return ret;
    }

    const std::vector<uint8>& data = font->GetData();

    FT_Face nativeFace = nullptr;

    const FT_Error error = FT_New_Memory_Face(
        library,
        reinterpret_cast<const FT_Byte*>(data.data()),
        static_cast<FT_Long>(data.size()),
        0,
        &nativeFace);

    if (error != FT_Err_Ok)
        return nullptr;

    if (FT_Select_Charmap(nativeFace, FT_ENCODING_UNICODE) != FT_Err_Ok)
    {
        FT_Done_Face(nativeFace);
        return nullptr;
    }

    auto newFace = std::make_unique<FontFace>(font, nativeFace);

    ret = newFace.get();
    faces.insert({ handle, std::move(newFace) });

    return ret;
}

FontAtlas* FontSystem::GetOrCreateAtlas(FontFace* face, uint32 pixelHeight)
{
    FontAtlas* ret = nullptr;

    const URay::Font* font = face->GetFont();

    const FontAtlasKey key = {
        .fontHandle = font->GetHandle(),
        .pixelHeight = pixelHeight
    };

    const auto it = atlases.find(key);
    if (it != atlases.end())
    {
        ret = it->second.get();
        return ret;
    }

    std::unique_ptr<FontAtlas> atlas = std::make_unique<FontAtlas>(face, pixelHeight);

    ret = atlas.get();
    atlases.insert({ key, std::move(atlas) });

    return ret;
}

const Glyph* FontSystem::GetOrCreateGlyph(FontAtlas* atlas, char32_t codepoint)
{
    const Glyph* ret = nullptr;

    ret = atlas->FindGlyph(codepoint);
    if (ret)
        return ret;

    FontFace* face = atlas->face;
    FT_Face nativeFace = face->nativeFace;

    if (FT_Set_Pixel_Sizes(nativeFace, 0, atlas->pixelHeight) != FT_Err_Ok)
        return nullptr;

    if (FT_Load_Char(nativeFace, static_cast<FT_ULong>(codepoint), FT_LOAD_RENDER) != FT_Err_Ok)
        return nullptr;

    const FT_GlyphSlot slot = nativeFace->glyph;
    const FT_Bitmap& bitmap = slot->bitmap;

    Glyph glyph = {};
    glyph.bearingX = slot->bitmap_left;
    glyph.bearingY = slot->bitmap_top;
    glyph.advance = static_cast<float>(slot->advance.x) / 64.0f;

    // Add glyph even the codepoint is blank. (For advance)
    if (bitmap.width == 0 || bitmap.rows == 0)
    {
        auto [it, inserted] = atlas->glyphs.emplace(codepoint, glyph);
        ret = &it->second;
        return ret;
    }

    const uint32 packedWidth = bitmap.width + atlas->padding * 2;
    const uint32 packedHeight = bitmap.rows + atlas->padding * 2;

    if (atlas->cursorX + packedWidth > atlas->width)
    {
        atlas->cursorX = 0;
        atlas->cursorY += atlas->rowHeight;
        atlas->rowHeight = 0;
    }

    if (atlas->cursorY + packedHeight > atlas->height)
        return nullptr;

    glyph.atlasX = atlas->cursorX + atlas->padding;
    glyph.atlasY = atlas->cursorY + atlas->padding;
    glyph.atlasWidth = bitmap.width;
    glyph.atlasHeight = bitmap.rows;

    for (uint32 y = 0; y < bitmap.rows; ++y)
    {
        const uint8* sourceRow = bitmap.buffer + y * bitmap.pitch;

        for (uint32 x = 0; x < bitmap.width; ++x)
        {
            const uint8 coverage = sourceRow[x];

            const size_t destinationIndex = ((glyph.atlasY + y) * atlas->width + (glyph.atlasX + x)) * 4;

            atlas->pixels[destinationIndex + 0] = 255;
            atlas->pixels[destinationIndex + 1] = 255;
            atlas->pixels[destinationIndex + 2] = 255;
            atlas->pixels[destinationIndex + 3] = coverage;
        }
    }

    atlas->cursorX += packedWidth;
    atlas->rowHeight = std::max(atlas->rowHeight, packedHeight);

    atlas->isDirty = true;

    auto [it, inserted] = atlas->glyphs.emplace(codepoint, glyph);

    ret = &it->second;
    return ret;
}

} // namespace URay::Render
