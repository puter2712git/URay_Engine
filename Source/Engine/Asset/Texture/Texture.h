#pragma once

#include "Engine/Asset/Asset.h"

#include "Core/Type/Types.h"

#include <string>
#include <vector>

namespace URay
{

class Texture : public Asset
{
    URAY_CLASS(Texture, Asset)

public:
    Texture(int32 width, int32 height, int32 channels,
            const std::vector<uint8>& pixels);
    ~Texture();

public:
    int32 GetWidth() const { return width; }
    int32 GetHeight() const { return height; }
    int32 GetChannels() const { return channels; }
    const std::vector<uint8>& GetPixels() const { return pixels; }

private:
    int32 width = 0;
    int32 height = 0;
    int32 channels = 0;

    std::vector<uint8> pixels;
};

} // namespace URay
