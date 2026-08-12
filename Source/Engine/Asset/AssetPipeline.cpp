#include "AssetPipeline.h"

#include "Engine/Importer/TextureImporter.h"
#include "Engine/Texture/Texture.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/UUID.h"

#include <stb/stb_image.h>

namespace URay
{

AssetPipeline::AssetPipeline(VirtualFilesystem& filesystem)
    : filesystem(filesystem)
{
    textureImporter = new TextureImporter(filesystem);
}

AssetPipeline::~AssetPipeline()
{
}

void AssetPipeline::Import(const VirtualPath& path)
{
    Texture* texture = textureImporter->Import(path);

    int width, height, channels;
    stbi_uc* pixels = stbi_load(texture->GetFilePath().c_str(), &width, &height, &channels, STBI_rgb_alpha);

    YAML::Node node = texture->Serialize();
    node["UUID"] = UUID::Generate().ToString();

    VirtualPath relativePath = path.GetRelativePath();
    VirtualPath importPath = VirtualPath("project://Asset/Imported/" + relativePath.ToString());
    std::string extension = path.GetExtension();
    importPath = importPath.ReplaceExtension(extension + ".props");
    filesystem.WriteText(importPath, YAML::Dump(node));
}

} // namespace URay
