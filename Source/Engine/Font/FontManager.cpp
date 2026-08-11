#include "FontManager.h"

#include "FontAsset.h"

#include "Core/File/FileIO.h"

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

FontAsset* FontManager::LoadFontAsset(const std::string& key, const std::string& filePath)
{
    if (!FileIO::Exists(filePath))
        return nullptr;

    FontAsset* fontAsset = new FontAsset(filePath);
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
