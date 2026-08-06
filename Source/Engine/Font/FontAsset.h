#pragma once

#include "Core/Math/Vector2.h"

#include <string>

namespace URay
{

class FontAsset
{
public:
    FontAsset(const std::string& filePath);

public:
    Vector2 GetUVFromChar(const char letter) const;

    const std::string& GetFilePath() const { return filePath; }

    float GetWidth() const { return width; }
    float GetHeight() const { return height; }

    float GetCellWidth() const { return width / column; }
    float GetCellHeight() const { return height / row; }

private:
    std::string filePath;

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
