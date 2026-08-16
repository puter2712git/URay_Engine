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
    TextureImporter* textureImporter = new TextureImporter(filesystem);
    importers.insert({ ".jpg", textureImporter });
    importers.insert({ ".png", textureImporter });
}

AssetPipeline::~AssetPipeline()
{
}

void AssetPipeline::Import(const VirtualPath& path)
{
    const std::string& fileExtension = path.GetExtension();

    auto it = importers.find(fileExtension);
    if (it == importers.end())
        return;

    Importer* importer = it->second;
    Object* importResult = importer->Import(path);

    YAML::Node serializedNode = importResult->Serialize();
    serializedNode["UUID"] = UUID::Generate().ToString();

    const VirtualPath relativePath = path.GetRelativePath();
    VirtualPath importPath = VirtualPath("Project://Asset/Imported/" + relativePath.ToString());
    importPath = importPath.ReplaceExtension(fileExtension + ".props");

    filesystem.WriteText(importPath, YAML::Dump(serializedNode));
}

} // namespace URay
