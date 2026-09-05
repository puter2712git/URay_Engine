#include "Texture.h"

#include "Core/Type/Types.h"

namespace URay
{

URAY_REGISTER_CLASS(Texture)

Texture::Texture(int32 width, int32 height, int32 channels,
                 const std::vector<uint8>& pixels)
    : width(width), height(height), channels(channels),
      pixels(pixels) {}

Texture::~Texture() {}

void Texture::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty({ .type = PropertyType::Int,
                                 .name = "Width",
                                 .offset = offsetof(Texture, width),
                                 .size = sizeof(int32) });
    StaticClass()->AddProperty({ .type = PropertyType::Int,
                                 .name = "Height",
                                 .offset = offsetof(Texture, height),
                                 .size = sizeof(int32) });
    StaticClass()->AddProperty({ .type = PropertyType::Int,
                                 .name = "Channels",
                                 .offset = offsetof(Texture, channels),
                                 .size = sizeof(int32) });
}

} // namespace URay
