#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <unordered_map>

namespace URay
{

class Font;
class Texture;

class FontManager
{
public:
    ~FontManager();

public:
    Font* LoadFont(const std::string& key, Texture* bitmapTexture);
    Font* GetFont(const std::string& key) const;

private:
    std::unordered_map<std::string, Font*> fonts;
};

} // namespace URay
