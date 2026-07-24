#include "TextureManager.h"

#include "Core/File/FileIO.h"

namespace URay
{

TextureAsset* TextureManager::LoadTextureAsset(const std::string& key, const std::string& filePath)
{
    if (!FileIO::Exists(filePath))
        return nullptr;
}

} // namespace URay
