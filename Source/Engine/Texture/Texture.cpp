#include "Texture.h"

namespace URay
{

Texture::Texture(
    const std::string& filePath,
    int width, int height, int channels)
    : filePath(filePath),
      width(width), height(height), channels(channels)
{
}

} // namespace URay
