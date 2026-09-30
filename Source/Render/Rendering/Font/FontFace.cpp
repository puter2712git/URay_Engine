#include "FontFace.h"

namespace URay::Render
{

FontFace::FontFace(const URay::Font* font, FT_Face nativeFace)
    : font(font), nativeFace(nativeFace) {}

FontFace::~FontFace()
{
    FT_Done_Face(nativeFace);
    nativeFace = nullptr;
}

} // namespace URay::Render
