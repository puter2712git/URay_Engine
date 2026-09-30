#include "FontSystem.h"

#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Font/Font.h"
#include "Engine/Engine.h"

#include "Render/RHI/RenderDevice.h"
#include "Render/RHI/Texture/Texture.h"
#include "Render/RHI/Texture/TextureView.h"

#include <algorithm>
#include <cstring>

namespace URay::Render
{

FontSystem::FontSystem(RenderDevice& device) : device(device) {};

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

bool FontSystem::FlushAtlasUploads()
{
    bool succeeded = true;

    for (auto& [key, atlas] : atlases)
    {
        if (!atlas->isDirty)
            continue;

        if (atlas->texture)
        {
            const TextureRegion& region = atlas->dirtyRegion;

            if (region.IsEmpty())
            {
                succeeded = false;
                continue;
            }

            const size_t rowByteSize = static_cast<size_t>(region.width) * 4;

            std::vector<uint8> packedPixels(rowByteSize * region.height);

            for (uint32 y = 0; y < region.height; ++y)
            {
                const size_t sourceIndex = (static_cast<size_t>(region.y + y) * atlas->width + region.x) * 4;

                uint8* destination = packedPixels.data() + rowByteSize * y;

                std::memcpy(destination, atlas->pixels.data() + sourceIndex, rowByteSize);
            }

            if (!device.UploadTextureRegion(
                    atlas->texture.get(),
                    region,
                    packedPixels))
            {
                succeeded = false;
                continue;
            }

            atlas->isDirty = false;
            atlas->dirtyRegion = {};

            continue;
        }

        const TextureDesc textureDesc = {
            .width = atlas->width,
            .height = atlas->height,
            .format = Format::RGBA8_UNorm,
            .usage = TextureUsage::TransferDst | TextureUsage::Sampled
        };

        atlas->texture.reset(device.CreateTexture(textureDesc));
        if (!atlas->texture)
        {
            succeeded = false;
            continue;
        }

        if (!device.UploadTextureData(atlas->texture.get(), atlas->pixels))
        {
            atlas->texture.reset();
            succeeded = false;
            continue;
        }

        atlas->view.reset(device.CreateTextureView(atlas->texture.get(), TextureViewDesc{}));
        if (!atlas->view)
        {
            atlas->texture.reset();
            succeeded = false;
            continue;
        }

        atlas->isDirty = false;
        atlas->dirtyRegion = {};
    }

    return succeeded;
}

FontFace* FontSystem::GetOrCreateFace(AssetHandle fontHandle)
{
    FontFace* ret = nullptr;

    const auto it = faces.find(fontHandle);
    if (it != faces.end())
    {
        ret = it->second.get();
        return ret;
    }

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    URay::Font* font = assetDatabase.Find<URay::Font>(fontHandle);
    if (!font)
        return nullptr;

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
    faces.insert({ fontHandle, std::move(newFace) });

    return ret;
}

FontAtlas* FontSystem::GetOrCreateAtlas(AssetHandle fontHandle, uint32 pixelHeight)
{
    FontAtlas* ret = nullptr;

    FontFace* face = GetOrCreateFace(fontHandle);
    if (!face)
        return nullptr;

    const URay::Font* font = face->font;

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

    const int32 rowCount = static_cast<int32>(bitmap.rows);
    const int32 rowPitch = bitmap.pitch;

    for (int32 y = 0; y < rowCount; ++y)
    {
        const uint8* sourceRow = bitmap.buffer + y * rowPitch;

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

    TextureRegion textureRegion = {};
    textureRegion.x = glyph.atlasX;
    textureRegion.y = glyph.atlasY;
    textureRegion.width = glyph.atlasWidth;
    textureRegion.height = glyph.atlasHeight;

    atlas->dirtyRegion = atlas->dirtyRegion.Union(textureRegion);

    auto [it, inserted] = atlas->glyphs.emplace(codepoint, glyph);

    ret = &it->second;
    return ret;
}

} // namespace URay::Render
