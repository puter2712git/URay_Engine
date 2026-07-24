#include "TextureAsset.h"

namespace URay
{

TextureAsset::TextureAsset(
    const std::string& filePath,
    int width, int height, int channels)
    : filePath(filePath),
      width(width), height(height), channels(channels)
{
}

} // namespace URay
