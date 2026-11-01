#pragma once

#include <Engine/Asset/Asset.h>

#include <Core/Type/TypeAlias.h>

#include <string>
#include <vector>

namespace URay
{

class Texture : public Asset
{
    URAY_OBJECT(Texture, Asset)

public:
    Texture(int32 width, int32 height, int32 channels,
            const Vector<uint8>& pixels);
    ~Texture() override;

public:
    int32 GetWidth() const { return width; }
    int32 GetHeight() const { return height; }
    int32 GetChannels() const { return channels; }
    const Vector<uint8>& GetPixels() const { return pixels; }

private:
    int32 width = 0;
    int32 height = 0;
    int32 channels = 0;

    Vector<uint8> pixels;
};

} // namespace URay
