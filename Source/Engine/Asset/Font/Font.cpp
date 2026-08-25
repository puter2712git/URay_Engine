#include "Font.h"

namespace URay
{

Font::Font(Texture* textureAsset)
    : bitmapTexture(textureAsset)
{
}

Vector2 Font::GetUVFromChar(const char letter) const
{
    int index = -1;

    for (size_t i = 0; i < charset.length(); ++i)
    {
        if (letter == charset[i])
        {
            index = i;
            break;
        }
    }

    if (index == -1)
        return Vector2::Zero;

    int r = index / column;
    int c = index % column;

    float startU = GetCellWidth() * c;
    float startV = GetCellHeight() * r;

    Vector2 uv = Vector2(startU / width, startV / height);
    return uv;
}

} // namespace URay
