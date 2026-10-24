#include "ShaderImporter.h"

#include <Core/File/VirtualFileSystem.h>
#include <Core/File/VirtualPath.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Importer/ImportRequest.h>
#include <Engine/Asset/Shader/Shader.h>
#include <Engine/Engine.h>

namespace URay
{

ShaderImporter::ShaderImporter(AssetSystem& assetSystem, VirtualFileSystem& fileSystem)
    : Importer(assetSystem, fileSystem) {}

ShaderImporter::~ShaderImporter() = default;

ImportResult ShaderImporter::Import(const ImportRequest& request)
{
    const VirtualPath metadataPath = assetSystem.GetMetadataPath(request.sourcePath);
    const AssetMetadata metadata = LoadOrCreateMetadata(request.sourcePath, metadataPath, Shader::StaticClass());

    ImportResult result = ImportResult::Success();
    result.metadatas.push_back(metadata);
    return result;

    // std::unique_ptr<Shader> shader = std::make_unique<Shader>(sourcePath);
    // shader->SetName(metadata.sourcePath.GetStem());
    // shader->SetHandle(metadata.handle);

    // assetDatabase.Add(std::move(shader));
}

bool ShaderImporter::CanImport(const std::string& extension) const
{
    return extension == ".hlsl";
}

} // namespace URay
