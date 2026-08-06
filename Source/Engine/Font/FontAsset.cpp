#include "FontAsset.h"

namespace URay
{

FontAsset::FontAsset(const std::string& filePath)
    : filePath(filePath)
{
}

Vector2 FontAsset::GetUVFromChar(const char letter) const
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
}

} // namespace URay
