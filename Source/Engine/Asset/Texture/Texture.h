#pragma once

#include "Engine/Object/Object.h"

#include "Core/Type/Types.h"

#include <string>
#include <vector>

namespace URay
{

class Texture : public Object
{
    URAY_CLASS(Texture, Object)

public:
    Texture(const std::string& filePath,
            int width, int height, int channels,
            const std::vector<uint8>& pixels);

public:
    void SetName(const std::string& inName) { name = inName; }
    const std::string& GetName() const { return name; }

    const std::string& GetFilePath() const { return filePath; }

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetChannels() const { return channels; }

    const std::vector<uint8>& GetPixels() const { return pixels; }

private:
    std::string name;
    std::string filePath;

    int width = 0;
    int height = 0;
    int channels = 0;

    std::vector<uint8> pixels;
};

} // namespace URay
