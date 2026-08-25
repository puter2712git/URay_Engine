#include "FontManager.h"

#include "Font.h"

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

Font* FontManager::LoadFont(const std::string& key, Texture* bitmapTexture)
{
    Font* fontAsset = new Font(bitmapTexture);
    fonts.insert({ key, fontAsset });
    return fontAsset;
}

Font* FontManager::GetFont(const std::string& key) const
{
    auto it = fonts.find(key);
    if (it != fonts.end())
        return it->second;

    return nullptr;
}

} // namespace URay
