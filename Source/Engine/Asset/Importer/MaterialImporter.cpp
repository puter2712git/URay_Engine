#include "MaterialImporter.h"

#include "Engine/Asset/AssetFactory.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Importer/ImportContext.h"
#include "Engine/Asset/Material/Material.h"
#include "Engine/Asset/Shader/Shader.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"

#include "Core/File/VirtualFilesystem.h"

#include <yaml-cpp/yaml.h>

namespace URay
{

MaterialImporter::MaterialImporter(VirtualFilesystem& filesystem)
    : filesystem(filesystem)
{
}

ImportResult MaterialImporter::Import(const VirtualPath& path, ImportContext& context)
{
    AssetSystem& assetSystem = context.GetAssetSystem();
    const VirtualPath importPath = assetSystem.GetImportAssetPath(path);
    const VirtualPath metaPath(importPath.ToString() + ".meta");
    const VirtualPath assetPath(importPath.ToString() + ".asset");

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

    std::vector<Shader*> shaders = assetSystem.FindAssets<Shader>();
    Shader* meshShader = nullptr;

    for (Shader* shader : shaders)
    {
        if (shader->GetName() == "Mesh")
        {
            meshShader = shader;
        }
    }

    Material* material = assetSystem.GetAssetFactory().CreateMaterial(metadata, meshShader);
    if (!material)
        return {};

    for (const MaterialCookParameter& parameter : cookData.parameters)
    {
        switch (parameter.type)
        {
        case MaterialCookParameterType::Float:
            material->SetParameter(
                parameter.name,
                { .type = MaterialParameterType::Float,
                  .value = std::get<float>(parameter.value) });
            break;
        case MaterialCookParameterType::Float2:
            material->SetParameter(
                parameter.name,
                { .type = MaterialParameterType::Float2,
                  .value = std::get<Vector2>(parameter.value) });
            break;
        case MaterialCookParameterType::Float3:
            material->SetParameter(
                parameter.name,
                { .type = MaterialParameterType::Float3,
                  .value = std::get<Vector3>(parameter.value) });
            break;
        case MaterialCookParameterType::Float4:
            material->SetParameter(
                parameter.name,
                { .type = MaterialParameterType::Float4,
                  .value = std::get<Color>(parameter.value) });
            break;
        case MaterialCookParameterType::Texture2D:
        {
            const VirtualPath& texturePath = std::get<VirtualPath>(parameter.value);
            const UUID textureUUID = assetSystem.Import(texturePath);
            if (Texture* texture = assetSystem.Find<Texture>(textureUUID))
            {
                material->SetParameter(
                    parameter.name,
                    { .type = MaterialParameterType::Texture2D,
                      .value = texture });
            }
            break;
        }
        }
    }

    return ImportResult{ .entries = { AssetEntry{ .asset = material, .metadata = metadata } } };
}

bool MaterialImporter::CanImport(const std::string& extension) const
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
        data.shaderUUID = UUID::FromString(node["Shader"].as<std::string>());

    if (const YAML::Node parameters = node["Parameters"])
    {
        if (!parameters.IsMap())
            return false;

        for (const auto& entry : parameters)
        {
            if (!entry.first.IsScalar() || !entry.second.IsMap() ||
                !entry.second["Type"] || !entry.second["Value"])
            {
                return false;
            }

            MaterialCookParameter parameter = {};
            parameter.name = entry.first.as<std::string>();
            const std::string type = entry.second["Type"].as<std::string>();
            const YAML::Node value = entry.second["Value"];

            if (type == "Float")
            {
                parameter.type = MaterialCookParameterType::Float;
                parameter.value = value.as<float>();
            }
            else if (type == "Float2")
            {
                if (!value.IsSequence() || value.size() != 2)
                    return false;

                parameter.type = MaterialCookParameterType::Float2;
                parameter.value = Vector2(value[0].as<float>(), value[1].as<float>());
            }
            else if (type == "Float3")
            {
                if (!value.IsSequence() || value.size() != 3)
                    return false;

                parameter.type = MaterialCookParameterType::Float3;
                parameter.value = Vector3(value[0].as<float>(), value[1].as<float>(), value[2].as<float>());
            }
            else if (type == "Float4")
            {
                if (!value.IsSequence() || value.size() != 4)
                    return false;

                parameter.type = MaterialCookParameterType::Float4;
                parameter.value = Color(value[0].as<float>(), value[1].as<float>(),
                                        value[2].as<float>(), value[3].as<float>());
            }
            else if (type == "Texture2D")
            {
                parameter.type = MaterialCookParameterType::Texture2D;
                parameter.value = VirtualPath(value.as<std::string>());
            }
            else
            {
                return false;
            }

            data.parameters.push_back(std::move(parameter));
        }

        return true;
    }

    if (const YAML::Node color = node["BaseColor"])
    {
        if (!color.IsSequence() || color.size() != 4)
            return false;

        data.parameters.push_back({
            .name = "baseColor",
            .type = MaterialCookParameterType::Float4,
            .value = Color(color[0].as<float>(), color[1].as<float>(),
                           color[2].as<float>(), color[3].as<float>()),
        });
    }

    if (node["BaseColorTexture"])
    {
        data.parameters.push_back({
            .name = "diffuseColorTexture",
            .type = MaterialCookParameterType::Texture2D,
            .value = VirtualPath(node["BaseColorTexture"].as<std::string>()),
        });
    }

    return true;
}

} // namespace URay
