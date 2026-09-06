#include "AssetPipeline.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Importer/ImportContext.h"
#include "Engine/Asset/Importer/MaterialImporter.h"
#include "Engine/Asset/Importer/ObjImporter.h"
#include "Engine/Asset/Importer/TextureImporter.h"

namespace URay
{

AssetPipeline::AssetPipeline(AssetSystem& assetSystem)
    : assetSystem(assetSystem), filesystem(assetSystem.GetFilesystem()) {}

AssetPipeline::~AssetPipeline()
{
}

bool AssetPipeline::Initialize()
{
    importers.push_back(std::make_unique<TextureImporter>(filesystem));
    importers.push_back(std::make_unique<MaterialImporter>(filesystem));
    importers.push_back(std::make_unique<ObjImporter>(filesystem));

    return true;
}

void AssetPipeline::Finalize()
{
    for (auto& importer : importers)
    {
        importer.reset();
    }
    importers.clear();
}

ImportResult AssetPipeline::Import(const VirtualPath& path)
{
    ImportResult importResult = {};

    const std::string extension = path.GetExtension();

    Importer* importer = FindImporter(extension);

    if (!importer)
        return {};

    ImportContext context = ImportContext(assetSystem);

    importResult = importer->Import(path, context);

    return importResult;
}

Importer* AssetPipeline::FindImporter(const std::string& extension) const
{
    for (auto& importer : importers)
    {
        if (importer->CanImport(extension))
        {
            return importer.get();
        }
    }

    return nullptr;
}

} // namespace URay
