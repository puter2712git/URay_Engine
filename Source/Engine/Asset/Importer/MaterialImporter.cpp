#include "MaterialImporter.h"

#include "Engine/Asset/AssetFactory.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Importer/ImportContext.h"
#include "Engine/Asset/Material/Material.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"

#include "Core/File/VirtualFilesystem.h"

#include "Render/RenderSystem.h"
#include "Render/Shader/ShaderManager.h"

#include <yaml-cpp/yaml.h>

namespace URay
{

MaterialImporter::MaterialImporter(VirtualFilesystem& filesystem)
    : filesystem(filesystem)
{
}

ImportResult MaterialImporter::Import(const VirtualPath& path, ImportContext& context)
{
    const VirtualPath metaPath("Asset://" + path.GetRelativePath() + ".meta");
    const VirtualPath assetPath("Asset://" + path.GetRelativePath() + ".asset");

    AssetMetadata metadata = {};
    if (!filesystem.Exists(metaPath))
    {
        metadata.uuid = UUID::Generate();
        metadata.type = AssetType::Material;
        metadata.sourcePath = path;
        metadata.importPath = assetPath;
        filesystem.WriteText(metaPath, YAML::Dump(metadata.Serialize()));
    }
    else
    {
        metadata.Deserialize(YAML::Load(filesystem.ReadText(metaPath)));
    }

    MaterialCookData cookData = {};
    if (!filesystem.Exists(assetPath))
    {
        if (!LoadSource(path, cookData))
            return {};
        filesystem.WriteBinary(assetPath, serializer.Serialize(cookData));
    }
    else if (!serializer.Deserialize(filesystem.ReadBinary(assetPath), cookData))
    {
        if (!LoadSource(path, cookData))
            return {};
        filesystem.WriteBinary(assetPath, serializer.Serialize(cookData));
    }

    AssetSystem& assetSystem = context.GetAssetSystem();
    Render::Shader* shader = gEngine->GetRenderSystem().GetShaderManager().GetOrCreate(cookData.shaderName);
    Material* material = assetSystem.GetAssetFactory().CreateMaterial(metadata, shader);
    material->SetBaseColor(cookData.baseColor);

    if (!cookData.baseColorTexturePath.ToString().empty())
    {
        const UUID textureUUID = assetSystem.Import(cookData.baseColorTexturePath);
        if (Texture* texture = assetSystem.Find<Texture>(textureUUID))
            material->SetTexture(texture);
    }

    return ImportResult{ .entries = { AssetEntry{ .asset = material, .metadata = metadata } } };
}

bool MaterialImporter::CanImport(const std::string& extension)
{
    return extension == ".mat";
}

bool MaterialImporter::LoadSource(const VirtualPath& path, MaterialCookData& data) const
{
    const YAML::Node node = YAML::Load(filesystem.ReadText(path));
    if (!node || !node["Type"] || node["Type"].as<std::string>() != "Material")
        return false;

    data = {};
    if (node["Shader"])
        data.shaderName = node["Shader"].as<std::string>();

    if (const YAML::Node color = node["BaseColor"])
    {
        if (!color.IsSequence() || color.size() != 4)
            return false;

        data.baseColor = { color[0].as<float>(), color[1].as<float>(),
                           color[2].as<float>(), color[3].as<float>() };
    }

    if (node["BaseColorTexture"])
        data.baseColorTexturePath = VirtualPath(node["BaseColorTexture"].as<std::string>());

    return true;
}

} // namespace URay
