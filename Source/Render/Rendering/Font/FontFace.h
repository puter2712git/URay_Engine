#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

namespace URay
{
class Font;
}

namespace URay::Render
{

class FontFace
{
    friend class FontSystem;

public:
    FontFace(const URay::Font* font, FT_Face nativeFace);
    ~FontFace();

private:
    const URay::Font* font = nullptr;
    FT_Face nativeFace = nullptr;
};

} // namespace URay::Render
