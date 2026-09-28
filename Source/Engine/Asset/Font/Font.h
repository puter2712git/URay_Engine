#pragma once

#include "Engine/Asset/Asset.h"

#include "Core/Math/Vector2.h"

#include <string>

namespace URay
{

class Texture;

class Font : public Asset
{
    URAY_CLASS(Font, Asset)

public:
    Font(const AssetHandle& bitmapTextureHandle);
    ~Font() override;

public:
    Vector2 GetUVFromChar(const char letter) const;

    const AssetHandle& GetBitmapTextureHandle() const { return bitmapTextureHandle; }

    float GetWidth() const { return width; }
    float GetHeight() const { return height; }

    float GetCellWidth() const { return width / column; }
    float GetCellHeight() const { return height / row; }

    float GetCellWidthUV() const { return GetCellWidth() / width; }
    float GetCellHeightUV() const { return GetCellHeight() / height; }

private:
    AssetHandle bitmapTextureHandle = {};

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
