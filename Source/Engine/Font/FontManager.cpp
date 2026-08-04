#include "FontManager.h"

#include "FontAsset.h"

namespace URay
{

FontManager::~FontManager()
{
    for (auto& [key, font] : fonts)
    {
        if (font)
        {
            delete font;
            font = nullptr;
        }
    }

    fonts.clear();
}

bool FontManager::Initialize()
{
    if (FT_Init_FreeType(&ft))
        return false;

    return true;
}

FontAsset* FontManager::LoadFontAsset(const std::string& key, const std::string& filePath)
{
    FT_Face face;

    if (FT_New_Face(ft, filePath.c_str(), 0, &face))
        return nullptr;

    FT_Set_Pixel_Sizes(face, 0, 48);

    FontAsset* fontAsset = new FontAsset(filePath);
    if (!fontAsset)
        return nullptr;

    fonts.insert({ key, fontAsset });
    return fontAsset;
}

FontAsset* FontManager::GetFontAsset(const std::string& key) const
{
    auto it = fonts.find(key);
    if (it != fonts.end())
        return it->second;

    return nullptr;
}

bool FontManager::GetGlyph(const std::string& key, const char letter) const
{
    auto it = faces.find(key);
    if (it == faces.end())
        return false;

    FT_Face face = it->second;
    if (!face)
        return false;

    FT_GlyphSlot slot = face->glyph;
    FT_Bitmap bitmap = slot->bitmap;

    return true;
}

} // namespace URay
