#pragma once

#include <string>

namespace URay
{

class TextureAsset
{
public:
    TextureAsset(const std::string& filePath,
                 int width, int height, int channels);

public:
    std::string GetFilePath() const { return filePath; }

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetChannels() const { return channels; }

private:
    std::string filePath;

    int width = 0;
    int height = 0;
    int channels = 0;
};

} // namespace URay
