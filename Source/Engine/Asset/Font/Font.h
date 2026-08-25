#pragma once

#include "Core/Math/Vector2.h"

#include <string>

namespace URay
{

class Texture;

class Font
{
public:
    Font(Texture* textureAsset);

public:
    Vector2 GetUVFromChar(const char letter) const;

    Texture* GetBitmapTexture() const { return bitmapTexture; }

    float GetWidth() const { return width; }
    float GetHeight() const { return height; }

    float GetCellWidth() const { return width / column; }
    float GetCellHeight() const { return height / row; }

    float GetCellWidthUV() const { return GetCellWidth() / width; }
    float GetCellHeightUV() const { return GetCellHeight() / height; }

private:
    Texture* bitmapTexture = nullptr;

    std::string charset =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "!1234567890@#$%^&*()-_=+:;'\"[{]}`~,.<>/?";

    float width = 512.0f;
    float height = 512.0f;

    int row = 16;
    int column = 16;
};

} // namespace URay
