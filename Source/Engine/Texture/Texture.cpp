#include "Texture.h"

namespace URay
{

URAY_REGISTER_CLASS(Texture)

Texture::Texture(
    const std::string& filePath,
    int width, int height, int channels)
    : filePath(filePath),
      width(width), height(height), channels(channels)
{
}

void Texture::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty({ PropertyType::Int, "Width", offsetof(Texture, width), sizeof(int) });
    StaticClass()->AddProperty({ PropertyType::Int, "Height", offsetof(Texture, height), sizeof(int) });
    StaticClass()->AddProperty({ PropertyType::Int, "Channels", offsetof(Texture, channels), sizeof(int) });
}

} // namespace URay
