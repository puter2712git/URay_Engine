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

FontAsset* FontManager::LoadFontAsset(const std::string& key, TextureAsset* bitmapTexture)
{
    FontAsset* fontAsset = new FontAsset(bitmapTexture);
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

} // namespace URay
