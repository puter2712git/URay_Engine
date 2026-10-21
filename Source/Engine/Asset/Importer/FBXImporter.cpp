#include "FBXImporter.h"

#include <Core/File/VirtualFileSystem.h>
#include <Core/Log/LogSystem.h>
#include <Core/Type/Types.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Asset/Material/Material.h>
#include <Engine/Asset/Mesh/Mesh.h>
#include <Engine/Engine.h>
#include <Render/RHI/Vertex/Vertex.h>

#include <ufbx/ufbx.h>
#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <filesystem>

namespace URay
{

void FBXImporter::Import(const VirtualPath& sourcePath)
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    VirtualFileSystem& fileSystem = assetSystem.GetFileSystem();

    AssetMetadata metadata = {};
    const VirtualPath importPath = assetSystem.GetImportAssetPath(sourcePath);
    const VirtualPath metadataPath(importPath.ToString() + ".meta");

    if (fileSystem.Exists(metadataPath))
    {
        metadata.Deserialize(YAML::Load(fileSystem.ReadText(metadataPath)));
    }
    else
    {
        metadata = CreateMetadata(sourcePath);
        fileSystem.WriteText(metadataPath, YAML::Dump(metadata.Serialize()));
    }

    assetSystem.RegisterMetadata(metadata);

    const std::filesystem::path physicalPath = fileSystem.ResolveToPhysicalPath(sourcePath);

    ufbx_error error = {};

    ufbx_load_opts options = {};
    options.generate_missing_normals = true;
    options.target_axes = ufbx_axes_right_handed_z_up;
    options.target_unit_meters = 1.0f;
    options.space_conversion = UFBX_SPACE_CONVERSION_ADJUST_TRANSFORMS;

    ufbx_scene* scene = ufbx_load_file(
        physicalPath.string().c_str(),
        &options,
        &error);

    if (!scene)
    {
        char errorText[1024] = {};
        ufbx_format_error(errorText, sizeof(errorText), &error);

        URAY_LOG(
            "[FBXImporter] Failed to load '%s': %s",
            sourcePath.ToString().c_str(),
            errorText);
        return;
    }

    if (scene->meshes.count == 0)
    {
        URAY_LOG("[FBXImporter] No mesh found: %s",
                 sourcePath.ToString().c_str());
        ufbx_free_scene(scene);
        return;
    }

    CreateStaticMesh(*scene, metadata);

    ufbx_free_scene(scene);
}

AssetMetadata FBXImporter::CreateMetadata(const VirtualPath& sourcePath) const
{
    AssetMetadata metadata = {};

    AssetSystem& assetSystem = gEngine->GetAssetSystem();

    metadata.handle = AssetHandle::Generate();
    metadata.type = AssetType::Mesh;
    metadata.sourcePath = sourcePath;

    const VirtualPath importPath = assetSystem.GetImportAssetPath(sourcePath);
    metadata.metadataPath = VirtualPath(importPath.ToString() + ".meta");
    metadata.assetPath = VirtualPath(importPath.ToString() + ".asset");

    return metadata;
}

bool FBXImporter::CanImport(const std::string& extension) const
{
    return extension == ".fbx";
}

bool FBXImporter::CreateStaticMesh(const ufbx_scene& sourceScene, const AssetMetadata& metadata) const
{
    std::vector<Render::VertexPNT> vertices;
    std::vector<uint32> indices;
    std::vector<MeshSection> sections;
    std::vector<AssetHandle> defaultMaterials;

    for (size_t meshIndex = 0; meshIndex < sourceScene.meshes.count; ++meshIndex)
    {
        const ufbx_mesh& sourceMesh = *sourceScene.meshes.data[meshIndex];

        if (!sourceMesh.vertex_position.exists)
        {
            URAY_LOG(
                "[FBXImporter] Skipping mesh '%zu' in '%s': no vertex positions.",
                meshIndex,
                metadata.sourcePath.ToString().c_str());
            continue;
        }

        const std::vector<AssetHandle> sourceMaterialHandles = CreateMaterials(sourceMesh, metadata);

        const size_t triangleIndexCapacity = std::max<size_t>(3, sourceMesh.max_face_triangles * 3);

        std::vector<uint32> triangleCornerIndices(triangleIndexCapacity);

        for (size_t instanceIndex = 0; instanceIndex < sourceMesh.instances.count; ++instanceIndex)
        {
            const ufbx_node& sourceNode = *sourceMesh.instances.data[instanceIndex];

            const ufbx_matrix normalMatrix = ufbx_matrix_for_normals(&sourceNode.geometry_to_world);

            for (size_t partListIndex = 0; partListIndex < sourceMesh.material_parts.count; ++partListIndex)
            {
                const ufbx_mesh_part& materialPart = sourceMesh.material_parts.data[partListIndex];

                MeshSection section = {};
                section.indexOffset = static_cast<uint32>(indices.size());

                for (size_t i = 0; i < materialPart.face_indices.count; ++i)
                {
                    const uint32 faceIndex = materialPart.face_indices.data[i];
                    const ufbx_face face = sourceMesh.faces.data[faceIndex];

                    if (face.num_indices < 3)
                        continue;

                    const uint32 triangleCount =
                        ufbx_triangulate_face(
                            triangleCornerIndices.data(),
                            triangleCornerIndices.size(),
                            &sourceMesh,
                            face);

                    if (triangleCount == 0)
                    {
                        URAY_LOG(
                            "[FBXImporter] Failed to triangulate face %u in '%s'.",
                            faceIndex,
                            metadata.sourcePath.ToString().c_str());
                        continue;
                    }

                    for (uint32 triangleIndex = 0; triangleIndex < triangleCount * 3; ++triangleIndex)
                    {
                        const uint32 cornerIndex = triangleCornerIndices[triangleIndex];

                        const ufbx_vec3 sourcePosition =
                            ufbx_get_vertex_vec3(&sourceMesh.vertex_position, cornerIndex);

                        const ufbx_vec3 worldPosition =
                            ufbx_transform_position(&sourceNode.geometry_to_world, sourcePosition);

                        Render::VertexPNT vertex = {};

                        vertex.position = Vector3(
                            static_cast<float>(worldPosition.x),
                            static_cast<float>(worldPosition.y),
                            static_cast<float>(worldPosition.z));

                        if (sourceMesh.vertex_normal.exists)
                        {
                            const ufbx_vec3 sourceNormal =
                                ufbx_get_vertex_vec3(&sourceMesh.vertex_normal, cornerIndex);

                            const ufbx_vec3 worldNormal =
                                ufbx_transform_position(&normalMatrix, sourceNormal);

                            vertex.normal = Vector3(
                                                static_cast<float>(worldNormal.x),
                                                static_cast<float>(worldNormal.y),
                                                static_cast<float>(worldNormal.z))
                                                .GetNormalized();
                        }
                        else
                        {
                            vertex.normal = Vector3::Up;
                        }

                        if (sourceMesh.vertex_uv.exists)
                        {
                            const ufbx_vec2 sourceUV =
                                ufbx_get_vertex_vec2(&sourceMesh.vertex_uv, cornerIndex);

                            vertex.uv = Vector2(
                                static_cast<float>(sourceUV.x),
                                1.0f - static_cast<float>(sourceUV.y));
                        }
                        else
                        {
                            vertex.uv = Vector2::Zero;
                        }

                        const uint32 vertexIndex = static_cast<uint32>(vertices.size());

                        vertices.push_back(vertex);
                        indices.push_back(vertexIndex);
                    }
                }

                section.indexCount = static_cast<uint32>(indices.size()) - section.indexOffset;

                if (section.indexCount == 0)
                    continue;

                section.materialIndex = defaultMaterials.size();
                sections.push_back(section);

                const uint32 sourceMaterialIndex = materialPart.index;

                if (sourceMaterialIndex < sourceMaterialHandles.size())
                {
                    defaultMaterials.push_back(sourceMaterialHandles[sourceMaterialIndex]);
                }
                else
                {
                    defaultMaterials.push_back(EngineAsset::MeshMaterial);
                }
            }
        }
    }

    if (vertices.empty() || indices.empty())
    {
        URAY_LOG(
            "[FBXImporter] No renderable triangles in '%s'.",
            metadata.sourcePath.ToString().c_str());
        return false;
    }

    auto mesh = std::make_unique<Mesh>();

    mesh->SetHandle(metadata.handle);
    mesh->SetName(metadata.sourcePath.GetStem());
    mesh->SetVertices(vertices);
    mesh->SetIndices(indices);
    mesh->SetSections(sections);
    mesh->SetDefaultMaterials(defaultMaterials);

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    assetDatabase.Add(std::move(mesh));

    URAY_LOG(
        "[FBXImporter] Created merged static mesh '%s' "
        "(%zu vertices, %zu indices, %zu sections).",
        metadata.sourcePath.ToString().c_str(),
        vertices.size(),
        indices.size(),
        sections.size());

    return true;
}

std::vector<AssetHandle> FBXImporter::CreateMaterials(const ufbx_mesh& sourceMesh, const AssetMetadata& metadata) const
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();
    VirtualFileSystem& fileSystem = assetSystem.GetFileSystem();

    const VirtualPath directory = metadata.sourcePath.GetDirectory();

    std::vector<AssetHandle> materialHandles;
    materialHandles.reserve(sourceMesh.materials.count);

    for (size_t i = 0; i < sourceMesh.materials.count; ++i)
    {
        const ufbx_material* sourceMaterial = sourceMesh.materials.data[i];

        if (!sourceMaterial)
        {
            materialHandles.push_back(EngineAsset::MeshMaterial);
            continue;
        }

        auto material = std::make_unique<Material>(EngineAsset::MeshShader);

        const std::string sourceName(sourceMaterial->name.data, sourceMaterial->name.length);

        material->SetName(sourceName.empty()
                              ? metadata.sourcePath.GetStem() + "_Material_" + std::to_string(i)
                              : sourceName);
        material->SetHandle(AssetHandle::Generate());

        const ufbx_material_map* diffuseMap = nullptr;

        if (sourceMaterial->pbr.base_color.texture &&
            sourceMaterial->pbr.base_color.texture_enabled)
        {
            diffuseMap = &sourceMaterial->pbr.base_color;
        }
        else if (sourceMaterial->fbx.diffuse_color.texture &&
                 sourceMaterial->fbx.diffuse_color.texture_enabled)
        {
            diffuseMap = &sourceMaterial->fbx.diffuse_color;
        }

        if (diffuseMap)
        {
            const ufbx_texture* sourceTexture = diffuseMap->texture;

            const std::string relativeTexturePath(
                sourceTexture->relative_filename.data,
                sourceTexture->relative_filename.length);

            if (!relativeTexturePath.empty())
            {
                const VirtualPath diffuseTexturePath = directory.Join(relativeTexturePath);

                const VirtualPath diffuseTextureImportPath = assetSystem.GetImportAssetPath(diffuseTexturePath);
                const VirtualPath diffuseTextureMetadataPath(diffuseTextureImportPath.ToString() + ".meta");

                const std::string metadataText = fileSystem.ReadText(diffuseTextureMetadataPath);

                if (!metadataText.empty())
                {
                    AssetMetadata textureMetadata = {};
                    textureMetadata.Deserialize(YAML::Load(metadataText));

                    material->AddParameter(
                        "diffuseColorTexture",
                        MaterialParameterType::Texture2D,
                        textureMetadata.handle);
                }
            }
        }

        const AssetHandle handle = material->GetHandle();
        assetDatabase.Add(std::move(material));

        materialHandles.push_back(handle);
    }

    return materialHandles;
}

} // namespace URay
