#include "Texture.h"

namespace URay
{

URAY_REGISTER_CLASS(Texture)

Texture::Texture(
    const std::string& filePath,
    int width, int height, int channels,
    const std::vector<uint8_t>& pixels)
    : filePath(filePath),
      width(width), height(height), channels(channels),
      pixels(pixels)
{
}

void Texture::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty({ .type = PropertyType::Int,
                                 .name = "Width",
                                 .offset = offsetof(Texture, width),
                                 .size = sizeof(int) });
    StaticClass()->AddProperty({ .type = PropertyType::Int,
                                 .name = "Height",
                                 .offset = offsetof(Texture, height),
                                 .size = sizeof(int) });
    StaticClass()->AddProperty({ .type = PropertyType::Int,
                                 .name = "Channels",
                                 .offset = offsetof(Texture, channels),
                                 .size = sizeof(int) });
}

} // namespace URay
