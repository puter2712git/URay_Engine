#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <unordered_map>

namespace URay
{

class FontAsset;

class FontManager
{
public:
    ~FontManager();

public:
    FontAsset* LoadFontAsset(const std::string& key, const std::string& filePath);
    FontAsset* GetFontAsset(const std::string& key) const;

private:
    std::unordered_map<std::string, FontAsset*> fonts;
};

} // namespace URay
