#include "FontAtlas.h"

#include "Render/RHI/Texture/Texture.h"
#include "Render/RHI/Texture/TextureView.h"

namespace URay::Render
{

FontAtlas::FontAtlas(FontFace* face, uint32 pixelHeight)
    : face(face), pixelHeight(pixelHeight)
{
    pixels.resize(width * height * 4, 0);
}

FontAtlas::~FontAtlas() = default;

const Glyph* FontAtlas::FindGlyph(char32_t codepoint) const
{
    const auto it = glyphs.find(codepoint);
    return it != glyphs.end() ? &it->second : nullptr;
}

} // namespace URay::Render
