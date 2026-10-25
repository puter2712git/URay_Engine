#include "FBXImporter.h"
#include <Core/Type/Types.h>
#include <Core/Math/Math.h>

#include <Core/File/VirtualFileSystem.h>
#include <Core/Log/LogSystem.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Asset/Importer/ImportRequest.h>
#include <Engine/Asset/Material/Material.h>
#include <Engine/Asset/Mesh/Mesh.h>
#include <Engine/Asset/Mesh/SkinnedMesh.h>
#include <Engine/Engine.h>
#include <Render/RHI/Vertex/Vertex.h>

#include <ufbx/ufbx.h>
#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>

namespace URay
{

namespace
{
Matrix ToMatrix(const ufbx_matrix& source);
} // namespace

FBXImporter::FBXImporter(AssetSystem& assetSystem, VirtualFileSystem& fileSystem)
    : Importer(assetSystem, fileSystem) {}

FBXImporter::~FBXImporter() {}

ImportResult FBXImporter::Import(const ImportRequest& request)
{
    const VirtualPath metadataPath = assetSystem.GetMetadataPath(request.sourcePath);
    const AssetMetadata metadata = LoadOrCreateMetadata(request.sourcePath, metadataPath, Mesh::StaticClass());

    ImportResult result = ImportResult::Failure("Failed to import FBX file");
    return result;

    // assetSystem.RegisterMetadata(metadata);

    // const std::filesystem::path physicalPath = fileSystem.ResolveToPhysicalPath(sourcePath);

    // ufbx_error error = {};

    // ufbx_load_opts options = {};
    // options.generate_missing_normals = true;
    // options.target_axes = ufbx_axes_right_handed_z_up;
    // options.target_unit_meters = 1.0f;
    // options.space_conversion = UFBX_SPACE_CONVERSION_ADJUST_TRANSFORMS;

    // ufbx_scene* scene = ufbx_load_file(
    //     physicalPath.string().c_str(),
    //     &options,
    //     &error);

    // if (!scene)
    //{
    //     char errorText[1024] = {};
    //     ufbx_format_error(errorText, sizeof(errorText), &error);

    //    URAY_LOG(
    //        "[FBXImporter] Failed to load '%s': %s",
    //        sourcePath.ToString().c_str(),
    //        errorText);
    //    return;
    //}

    // if (scene->meshes.count == 0)
    //{
    //     URAY_LOG("[FBXImporter] No mesh found: %s",
    //              sourcePath.ToString().c_str());
    //     ufbx_free_scene(scene);
    //     return;
    // }

    // const FBX::FBXInfo fbxInfo = ParseFBXInfo(*scene);

    // size_t skinnedMeshCount = 0;
    // size_t staticMeshCount = 0;
    // for (size_t meshIndex = 0; meshIndex < scene->meshes.count; ++meshIndex)
    //{
    //     const ufbx_mesh* sourceMesh = scene->meshes.data[meshIndex];

    //    if (sourceMesh->skin_deformers.count > 0)
    //    {
    //        ++skinnedMeshCount;
    //    }
    //    else
    //    {
    //        ++staticMeshCount;
    //    }
    //}

    // bool imported = false;

    // if (skinnedMeshCount == 0)
    //{
    //     imported = CreateStaticMesh(*scene, metadata);
    // }
    // else
    //{
    //     if (staticMeshCount > 0)
    //     {
    //         URAY_LOG(
    //             "[FBXImporter] Skipping %zu static meshes in skinned FBX '%s'.",
    //             staticMeshCount,
    //             sourcePath.ToString().c_str());
    //     }

    //    imported = CreateSkinnedMesh(*scene, fbxInfo, metadata);
    //}

    // if (!imported)
    //{
    //     URAY_LOG(
    //         "[FBXImporter] Failed to create mesh asset for '%s'.",
    //         sourcePath.ToString().c_str());
    // }

    // ufbx_free_scene(scene);
}

bool FBXImporter::CanImport(const String& extension) const
{
    return extension == ".fbx";
}

// FBX::FBXInfo FBXImporter::ParseFBXInfo(const ufbx_scene& sourceScene) const
//{
//     FBX::FBXInfo fbxInfo = {};
//
//     std::unordered_set<const ufbx_node*> skeletonNodes;
//
//     for (size_t meshIndex = 0;
//          meshIndex < sourceScene.meshes.count;
//          ++meshIndex)
//     {
//         const ufbx_mesh& sourceMesh =
//             *sourceScene.meshes.data[meshIndex];
//
//         if (sourceMesh.skin_deformers.count == 0)
//             continue;
//
//         const ufbx_skin_deformer& skinDeformer =
//             *sourceMesh.skin_deformers.data[0];
//
//         for (size_t clusterIndex = 0;
//              clusterIndex < skinDeformer.clusters.count;
//              ++clusterIndex)
//         {
//             const ufbx_skin_cluster& cluster =
//                 *skinDeformer.clusters.data[clusterIndex];
//
//             for (const ufbx_node* node = cluster.bone_node;
//                  node;
//                  node = node->parent)
//             {
//                 skeletonNodes.insert(node);
//             }
//         }
//     }
//
//     std::unordered_map<const ufbx_node*, int32> boneIndices;
//
//     for (size_t nodeIndex = 0; nodeIndex < sourceScene.nodes.count; ++nodeIndex)
//     {
//         const ufbx_node* sourceNode = sourceScene.nodes.data[nodeIndex];
//
//         if (skeletonNodes.find(sourceNode) == skeletonNodes.end())
//             continue;
//
//         FBX::BoneInfo bone = {};
//         bone.name = String(sourceNode->name.data, sourceNode->name.length);
//         bone.worldBindTransform = ToMatrix(sourceNode->node_to_world);
//
//         const int32 boneIndex = static_cast<int32>(fbxInfo.bones.size());
//
//         fbxInfo.bones.push_back(std::move(bone));
//         boneIndices.emplace(sourceNode, boneIndex);
//     }
//
//     for (size_t nodeIndex = 0; nodeIndex < sourceScene.nodes.count; ++nodeIndex)
//     {
//         const ufbx_node* sourceNode = sourceScene.nodes.data[nodeIndex];
//
//         const auto boneIt = boneIndices.find(sourceNode);
//
//         if (boneIt == boneIndices.end())
//             continue;
//
//         FBX::BoneInfo& destinationBone = fbxInfo.bones[boneIt->second];
//         const ufbx_node* parentNode = sourceNode->parent;
//
//         while (parentNode)
//         {
//             const auto parentIt = boneIndices.find(parentNode);
//
//             if (parentIt != boneIndices.end())
//             {
//                 destinationBone.parentIndex = parentIt->second;
//                 break;
//             }
//
//             parentNode = parentNode->parent;
//         }
//     }
//
//     fbxInfo.meshes.reserve(sourceScene.meshes.count);
//
//     for (size_t meshIndex = 0; meshIndex < sourceScene.meshes.count; ++meshIndex)
//     {
//         const ufbx_mesh& sourceMesh = *sourceScene.meshes.data[meshIndex];
//
//         FBX::MeshInfo meshInfo = {};
//
//         meshInfo.name = String(sourceMesh.name.data, sourceMesh.name.length);
//
//         if (sourceMesh.skin_deformers.count == 0)
//         {
//             fbxInfo.meshes.push_back(std::move(meshInfo));
//             continue;
//         }
//
//         if (sourceMesh.skin_deformers.count > 1)
//         {
//             URAY_LOG(
//                 "[FBXImporter] Mesh '%.*s' has %zu skin deformers; "
//                 "only the first is currently used.",
//                 static_cast<int>(sourceMesh.name.length),
//                 sourceMesh.name.data,
//                 sourceMesh.skin_deformers.count);
//         }
//
//         const ufbx_skin_deformer& skinDeformer = *sourceMesh.skin_deformers.data[0];
//
//         meshInfo.clusters.resize(skinDeformer.clusters.count);
//
//         for (size_t clusterIndex = 0; clusterIndex < skinDeformer.clusters.count; ++clusterIndex)
//         {
//             const ufbx_skin_cluster& cluster = *skinDeformer.clusters.data[clusterIndex];
//
//             const auto boneIt = boneIndices.find(cluster.bone_node);
//
//             if (boneIt == boneIndices.end())
//             {
//                 URAY_LOG(
//                     "[FBXImporter] Mesh '%.*s': cluster %zu "
//                     "has no mapped skeleton bone.",
//                     static_cast<int>(sourceMesh.name.length),
//                     sourceMesh.name.data,
//                     clusterIndex);
//                 continue;
//             }
//
//             meshInfo.clusters[clusterIndex].boneIndex = static_cast<uint32>(boneIt->second);
//             meshInfo.clusters[clusterIndex].inverseBindMatrix = ToMatrix(cluster.geometry_to_bone);
//         }
//
//         meshInfo.skinVertices.resize(sourceMesh.num_vertices);
//
//         const size_t skinVertexCount = Math::Min(sourceMesh.num_vertices, skinDeformer.vertices.count);
//
//         size_t weightedVertexCount = 0;
//         size_t truncatedVertexCount = 0;
//
//         for (size_t logicalVertexIndex = 0; logicalVertexIndex < skinVertexCount; ++logicalVertexIndex)
//         {
//             const ufbx_skin_vertex& sourceSkinVertex = skinDeformer.vertices.data[logicalVertexIndex];
//
//             FBX::SkinVertex& destinationVertex = meshInfo.skinVertices[logicalVertexIndex];
//
//             if (sourceSkinVertex.num_weights > 4)
//             {
//                 ++truncatedVertexCount;
//             }
//
//             float weightSum = 0.0f;
//
//             for (size_t weightOffset = 0; weightOffset < sourceSkinVertex.num_weights && destinationVertex.count < 4; ++weightOffset)
//             {
//                 const size_t sourceWeightIndex = static_cast<size_t>(sourceSkinVertex.weight_begin) + weightOffset;
//
//                 if (sourceWeightIndex >= skinDeformer.weights.count)
//                     break;
//
//                 const ufbx_skin_weight& sourceWeight = skinDeformer.weights.data[sourceWeightIndex];
//
//                 if (sourceWeight.cluster_index >= meshInfo.clusters.size())
//                     continue;
//
//                 const uint32 boneIndex = meshInfo.clusters[sourceWeight.cluster_index].boneIndex;
//
//                 if (boneIndex == FBX::InvalidBoneIndex || sourceWeight.weight <= 0.0f)
//                     continue;
//
//                 FBX::SkinInfluence& destinationInfluence = destinationVertex.influences[destinationVertex.count];
//
//                 destinationInfluence.boneIndex = boneIndex;
//                 destinationInfluence.weight = static_cast<float>(sourceWeight.weight);
//
//                 weightSum += destinationInfluence.weight;
//                 ++destinationVertex.count;
//             }
//
//             if (destinationVertex.count == 0)
//                 continue;
//
//             ++weightedVertexCount;
//
//             for (uint32 influenceIndex = 0; influenceIndex < destinationVertex.count; ++influenceIndex)
//             {
//                 destinationVertex.influences[influenceIndex].weight /= weightSum;
//             }
//         }
//
//         fbxInfo.meshes.push_back(std::move(meshInfo));
//     }
//
//     return fbxInfo;
// }
//
// bool FBXImporter::CreateStaticMesh(const ufbx_scene& sourceScene, const AssetMetadata& metadata) const
//{
//     std::vector<Render::VertexPNT> vertices;
//     std::vector<uint32> indices;
//     std::vector<MeshSection> sections;
//     std::vector<AssetHandle> defaultMaterials;
//
//     for (size_t meshIndex = 0; meshIndex < sourceScene.meshes.count; ++meshIndex)
//     {
//         const ufbx_mesh& sourceMesh = *sourceScene.meshes.data[meshIndex];
//
//         if (!sourceMesh.vertex_position.exists)
//         {
//             URAY_LOG(
//                 "[FBXImporter] Skipping mesh '%zu' in '%s': no vertex positions.",
//                 meshIndex,
//                 metadata.sourcePath.ToString().c_str());
//             continue;
//         }
//
//         const std::vector<AssetHandle> sourceMaterialHandles = CreateMaterials(sourceMesh, metadata);
//
//         const size_t triangleIndexCapacity = Math::Max<size_t>(3, sourceMesh.max_face_triangles * 3);
//
//         std::vector<uint32> triangleCornerIndices(triangleIndexCapacity);
//
//         for (size_t instanceIndex = 0; instanceIndex < sourceMesh.instances.count; ++instanceIndex)
//         {
//             const ufbx_node& sourceNode = *sourceMesh.instances.data[instanceIndex];
//
//             const ufbx_matrix normalMatrix = ufbx_matrix_for_normals(&sourceNode.geometry_to_world);
//
//             for (size_t partListIndex = 0; partListIndex < sourceMesh.material_parts.count; ++partListIndex)
//             {
//                 const ufbx_mesh_part& materialPart = sourceMesh.material_parts.data[partListIndex];
//
//                 MeshSection section = {};
//                 section.indexOffset = static_cast<uint32>(indices.size());
//
//                 for (size_t i = 0; i < materialPart.face_indices.count; ++i)
//                 {
//                     const uint32 faceIndex = materialPart.face_indices.data[i];
//                     const ufbx_face face = sourceMesh.faces.data[faceIndex];
//
//                     if (face.num_indices < 3)
//                         continue;
//
//                     const uint32 triangleCount =
//                         ufbx_triangulate_face(
//                             triangleCornerIndices.data(),
//                             triangleCornerIndices.size(),
//                             &sourceMesh,
//                             face);
//
//                     if (triangleCount == 0)
//                     {
//                         URAY_LOG(
//                             "[FBXImporter] Failed to triangulate face %u in '%s'.",
//                             faceIndex,
//                             metadata.sourcePath.ToString().c_str());
//                         continue;
//                     }
//
//                     for (uint32 triangleIndex = 0; triangleIndex < triangleCount * 3; ++triangleIndex)
//                     {
//                         const uint32 cornerIndex = triangleCornerIndices[triangleIndex];
//
//                         const ufbx_vec3 sourcePosition =
//                             ufbx_get_vertex_vec3(&sourceMesh.vertex_position, cornerIndex);
//
//                         const ufbx_vec3 worldPosition =
//                             ufbx_transform_position(&sourceNode.geometry_to_world, sourcePosition);
//
//                         Render::VertexPNT vertex = {};
//
//                         vertex.position = Vector3(
//                             static_cast<float>(worldPosition.x),
//                             static_cast<float>(worldPosition.y),
//                             static_cast<float>(worldPosition.z));
//
//                         if (sourceMesh.vertex_normal.exists)
//                         {
//                             const ufbx_vec3 sourceNormal =
//                                 ufbx_get_vertex_vec3(&sourceMesh.vertex_normal, cornerIndex);
//
//                             const ufbx_vec3 worldNormal =
//                                 ufbx_transform_position(&normalMatrix, sourceNormal);
//
//                             vertex.normal = Vector3(
//                                                 static_cast<float>(worldNormal.x),
//                                                 static_cast<float>(worldNormal.y),
//                                                 static_cast<float>(worldNormal.z))
//                                                 .GetNormalized();
//                         }
//                         else
//                         {
//                             vertex.normal = Vector3::Up;
//                         }
//
//                         if (sourceMesh.vertex_uv.exists)
//                         {
//                             const ufbx_vec2 sourceUV =
//                                 ufbx_get_vertex_vec2(&sourceMesh.vertex_uv, cornerIndex);
//
//                             vertex.uv = Vector2(
//                                 static_cast<float>(sourceUV.x),
//                                 1.0f - static_cast<float>(sourceUV.y));
//                         }
//                         else
//                         {
//                             vertex.uv = Vector2::Zero;
//                         }
//
//                         const uint32 vertexIndex = static_cast<uint32>(vertices.size());
//
//                         vertices.push_back(vertex);
//                         indices.push_back(vertexIndex);
//                     }
//                 }
//
//                 section.indexCount = static_cast<uint32>(indices.size()) - section.indexOffset;
//
//                 if (section.indexCount == 0)
//                     continue;
//
//                 section.materialIndex = defaultMaterials.size();
//                 sections.push_back(section);
//
//                 const uint32 sourceMaterialIndex = materialPart.index;
//
//                 if (sourceMaterialIndex < sourceMaterialHandles.size())
//                 {
//                     defaultMaterials.push_back(sourceMaterialHandles[sourceMaterialIndex]);
//                 }
//                 else
//                 {
//                     defaultMaterials.push_back(EngineAsset::MeshMaterial);
//                 }
//             }
//         }
//     }
//
//     if (vertices.empty() || indices.empty())
//     {
//         URAY_LOG(
//             "[FBXImporter] No renderable triangles in '%s'.",
//             metadata.sourcePath.ToString().c_str());
//         return false;
//     }
//
//     auto mesh = std::make_unique<Mesh>();
//
//     mesh->SetHandle(metadata.handle);
//     mesh->SetName(metadata.sourcePath.GetStem());
//     mesh->SetVertices(vertices);
//     mesh->SetIndices(indices);
//     mesh->SetSections(sections);
//     mesh->SetDefaultMaterials(defaultMaterials);
//
//     AssetSystem& assetSystem = gEngine->GetAssetSystem();
//     AssetDatabase& assetDatabase = assetSystem.GetDatabase();
//
//     assetDatabase.Add(std::move(mesh));
//
//     URAY_LOG(
//         "[FBXImporter] Created merged static mesh '%s' "
//         "(%zu vertices, %zu indices, %zu sections).",
//         metadata.sourcePath.ToString().c_str(),
//         vertices.size(),
//         indices.size(),
//         sections.size());
//
//     return true;
// }
//
// bool FBXImporter::CreateSkinnedMesh(
//     const ufbx_scene& sourceScene,
//     const FBX::FBXInfo& fbxInfo,
//     const AssetMetadata& metadata) const
//{
//     if (fbxInfo.bones.empty())
//     {
//         URAY_LOG(
//             "[FBXImporter] Skinned FBX '%s' has no skeleton bones.",
//             metadata.sourcePath.ToString().c_str());
//         return false;
//     }
//
//     int32 assetRootBoneIndex = -1;
//
//     for (const FBX::MeshInfo& meshInfo : fbxInfo.meshes)
//     {
//         for (const FBX::SkinClusterInfo& cluster : meshInfo.clusters)
//         {
//             if (cluster.boneIndex == FBX::InvalidBoneIndex ||
//                 cluster.boneIndex >= fbxInfo.bones.size())
//             {
//                 continue;
//             }
//
//             int32 rootBoneIndex = static_cast<int32>(cluster.boneIndex);
//
//             while (fbxInfo.bones[rootBoneIndex].parentIndex >= 0)
//             {
//                 rootBoneIndex = fbxInfo.bones[rootBoneIndex].parentIndex;
//             }
//
//             assetRootBoneIndex = rootBoneIndex;
//             break;
//         }
//
//         if (assetRootBoneIndex >= 0)
//             break;
//     }
//
//     if (assetRootBoneIndex < 0)
//     {
//         URAY_LOG(
//             "[FBXImporter] Skinned FBX '%s' has no bone clusters.",
//             metadata.sourcePath.ToString().c_str());
//         return false;
//     }
//
//     const Matrix& assetRootWorldBind =
//         fbxInfo.bones[assetRootBoneIndex].worldBindTransform;
//     const Matrix assetRootWorldInverse = assetRootWorldBind.Inverse();
//
//     std::vector<Render::VertexSkinnedPNT> vertices;
//     std::vector<uint32> indices;
//     std::vector<MeshSection> sections;
//     std::vector<AssetHandle> defaultMaterials;
//
//     for (size_t meshIndex = 0; meshIndex < sourceScene.meshes.count; ++meshIndex)
//     {
//         const ufbx_mesh& sourceMesh = *sourceScene.meshes.data[meshIndex];
//         const FBX::MeshInfo& meshInfo = fbxInfo.meshes[meshIndex];
//
//         if (sourceMesh.skin_deformers.count == 0)
//             continue;
//
//         if (!sourceMesh.vertex_position.exists)
//         {
//             URAY_LOG(
//                 "[FBXImporter] Skipping skinned mesh '%.*s' in '%s': no vertex positions.",
//                 static_cast<int>(sourceMesh.name.length),
//                 sourceMesh.name.data,
//                 metadata.sourcePath.ToString().c_str());
//             continue;
//         }
//
//         const std::vector<AssetHandle> sourceMaterialHandles =
//             CreateMaterials(sourceMesh, metadata);
//         const size_t triangleIndexCapacity =
//             Math::Max<size_t>(3, sourceMesh.max_face_triangles * 3);
//         std::vector<uint32> triangleCornerIndices(triangleIndexCapacity);
//
//         for (size_t instanceIndex = 0;
//              instanceIndex < sourceMesh.instances.count;
//              ++instanceIndex)
//         {
//             const ufbx_node& sourceNode =
//                 *sourceMesh.instances.data[instanceIndex];
//             const Matrix meshToAsset =
//                 ToMatrix(sourceNode.geometry_to_world) * assetRootWorldInverse;
//             const Matrix normalToAsset = meshToAsset.Inverse().Transpose();
//
//             for (size_t partListIndex = 0;
//                  partListIndex < sourceMesh.material_parts.count;
//                  ++partListIndex)
//             {
//                 const ufbx_mesh_part& materialPart =
//                     sourceMesh.material_parts.data[partListIndex];
//
//                 MeshSection section = {};
//                 section.indexOffset = static_cast<uint32>(indices.size());
//
//                 for (size_t faceListIndex = 0;
//                      faceListIndex < materialPart.face_indices.count;
//                      ++faceListIndex)
//                 {
//                     const uint32 faceIndex =
//                         materialPart.face_indices.data[faceListIndex];
//                     const ufbx_face face = sourceMesh.faces.data[faceIndex];
//
//                     if (face.num_indices < 3)
//                         continue;
//
//                     const uint32 triangleCount = ufbx_triangulate_face(
//                         triangleCornerIndices.data(),
//                         triangleCornerIndices.size(),
//                         &sourceMesh,
//                         face);
//
//                     if (triangleCount == 0)
//                     {
//                         URAY_LOG(
//                             "[FBXImporter] Failed to triangulate face %u in '%s'.",
//                             faceIndex,
//                             metadata.sourcePath.ToString().c_str());
//                         continue;
//                     }
//
//                     for (uint32 triangleIndex = 0;
//                          triangleIndex < triangleCount * 3;
//                          ++triangleIndex)
//                     {
//                         const uint32 cornerIndex =
//                             triangleCornerIndices[triangleIndex];
//
//                         if (cornerIndex >= sourceMesh.vertex_indices.count)
//                         {
//                             URAY_LOG(
//                                 "[FBXImporter] Invalid vertex corner %u in '%s'.",
//                                 cornerIndex,
//                                 metadata.sourcePath.ToString().c_str());
//                             return false;
//                         }
//
//                         const uint32 logicalVertexIndex =
//                             sourceMesh.vertex_indices.data[cornerIndex];
//
//                         if (logicalVertexIndex >= meshInfo.skinVertices.size())
//                         {
//                             URAY_LOG(
//                                 "[FBXImporter] Invalid skin vertex %u in '%s'.",
//                                 logicalVertexIndex,
//                                 metadata.sourcePath.ToString().c_str());
//                             return false;
//                         }
//
//                         const FBX::SkinVertex& skinVertex =
//                             meshInfo.skinVertices[logicalVertexIndex];
//
//                         if (skinVertex.count == 0)
//                         {
//                             URAY_LOG(
//                                 "[FBXImporter] Vertex %u in '%s' has no valid skin influence.",
//                                 logicalVertexIndex,
//                                 metadata.sourcePath.ToString().c_str());
//                             return false;
//                         }
//
//                         Render::VertexSkinnedPNT vertex = {};
//
//                         const ufbx_vec3 sourcePosition =
//                             ufbx_get_vertex_vec3(
//                                 &sourceMesh.vertex_position,
//                                 cornerIndex);
//                         vertex.position = meshToAsset.TransformPoint(Vector3(
//                             static_cast<float>(sourcePosition.x),
//                             static_cast<float>(sourcePosition.y),
//                             static_cast<float>(sourcePosition.z)));
//
//                         if (sourceMesh.vertex_normal.exists)
//                         {
//                             const ufbx_vec3 sourceNormal =
//                                 ufbx_get_vertex_vec3(
//                                     &sourceMesh.vertex_normal,
//                                     cornerIndex);
//                             vertex.normal = normalToAsset.TransformVector(Vector3(
//                                                                               static_cast<float>(sourceNormal.x),
//                                                                               static_cast<float>(sourceNormal.y),
//                                                                               static_cast<float>(sourceNormal.z)))
//                                                 .GetNormalized();
//                         }
//                         else
//                         {
//                             vertex.normal = Vector3::Up;
//                         }
//
//                         if (sourceMesh.vertex_uv.exists)
//                         {
//                             const ufbx_vec2 sourceUV =
//                                 ufbx_get_vertex_vec2(
//                                     &sourceMesh.vertex_uv,
//                                     cornerIndex);
//                             vertex.uv = Vector2(
//                                 static_cast<float>(sourceUV.x),
//                                 1.0f - static_cast<float>(sourceUV.y));
//                         }
//                         else
//                         {
//                             vertex.uv = Vector2::Zero;
//                         }
//
//                         for (uint32 influenceIndex = 0;
//                              influenceIndex < skinVertex.count;
//                              ++influenceIndex)
//                         {
//                             const FBX::SkinInfluence& influence =
//                                 skinVertex.influences[influenceIndex];
//
//                             if (influence.boneIndex >= fbxInfo.bones.size())
//                             {
//                                 URAY_LOG(
//                                     "[FBXImporter] Vertex %u in '%s' references invalid bone %u.",
//                                     logicalVertexIndex,
//                                     metadata.sourcePath.ToString().c_str(),
//                                     influence.boneIndex);
//                                 return false;
//                             }
//
//                             vertex.boneIndices[influenceIndex] = influence.boneIndex;
//                             vertex.boneWeights[influenceIndex] = influence.weight;
//                         }
//
//                         const uint32 vertexIndex =
//                             static_cast<uint32>(vertices.size());
//                         vertices.push_back(vertex);
//                         indices.push_back(vertexIndex);
//                     }
//                 }
//
//                 section.indexCount =
//                     static_cast<uint32>(indices.size()) - section.indexOffset;
//
//                 if (section.indexCount == 0)
//                     continue;
//
//                 section.materialIndex = defaultMaterials.size();
//                 sections.push_back(section);
//
//                 const uint32 sourceMaterialIndex = materialPart.index;
//
//                 if (sourceMaterialIndex < sourceMaterialHandles.size())
//                 {
//                     defaultMaterials.push_back(
//                         sourceMaterialHandles[sourceMaterialIndex]);
//                 }
//                 else
//                 {
//                     defaultMaterials.push_back(EngineAsset::MeshMaterial);
//                 }
//             }
//         }
//     }
//
//     if (vertices.empty() || indices.empty())
//     {
//         URAY_LOG(
//             "[FBXImporter] No renderable skinned triangles in '%s'.",
//             metadata.sourcePath.ToString().c_str());
//         return false;
//     }
//
//     auto mesh = std::make_unique<SkinnedMesh>();
//
//     mesh->SetHandle(metadata.handle);
//     mesh->SetName(metadata.sourcePath.GetStem());
//     mesh->SetVertices(vertices);
//     mesh->SetIndices(indices);
//     mesh->SetSections(sections);
//     mesh->SetDefaultMaterials(defaultMaterials);
//
//     std::vector<Bone> bones;
//     bones.reserve(fbxInfo.bones.size());
//
//     for (const FBX::BoneInfo& sourceBone : fbxInfo.bones)
//     {
//         Bone bone = {};
//         bone.name = sourceBone.name;
//         bone.parentIndex = sourceBone.parentIndex;
//
//         if (sourceBone.parentIndex >= 0)
//         {
//             bone.localBindTransform =
//                 sourceBone.worldBindTransform *
//                 fbxInfo.bones[sourceBone.parentIndex].worldBindTransform.Inverse();
//         }
//         else
//         {
//             bone.localBindTransform =
//                 sourceBone.worldBindTransform * assetRootWorldInverse;
//         }
//
//         bone.inverseBindMatrix =
//             assetRootWorldBind * sourceBone.worldBindTransform.Inverse();
//
//         bones.push_back(std::move(bone));
//     }
//
//     mesh->SetBones(bones);
//
//     AssetSystem& assetSystem = gEngine->GetAssetSystem();
//     AssetDatabase& assetDatabase = assetSystem.GetDatabase();
//
//     assetDatabase.Add(std::move(mesh));
//
//     URAY_LOG(
//         "[FBXImporter] Created skinned mesh '%s' "
//         "(%zu vertices, %zu indices, %zu sections).",
//         metadata.sourcePath.ToString().c_str(),
//         vertices.size(),
//         indices.size(),
//         sections.size());
//
//     return true;
// }
//
// std::vector<AssetHandle> FBXImporter::CreateMaterials(const ufbx_mesh& sourceMesh, const AssetMetadata& metadata) const
//{
//     AssetSystem& assetSystem = gEngine->GetAssetSystem();
//     AssetDatabase& assetDatabase = assetSystem.GetDatabase();
//     VirtualFileSystem& fileSystem = assetSystem.GetFileSystem();
//
//     const VirtualPath directory = metadata.sourcePath.GetDirectory();
//
//     std::vector<AssetHandle> materialHandles;
//     materialHandles.reserve(sourceMesh.materials.count);
//
//     for (size_t i = 0; i < sourceMesh.materials.count; ++i)
//     {
//         const ufbx_material* sourceMaterial = sourceMesh.materials.data[i];
//
//         if (!sourceMaterial)
//         {
//             materialHandles.push_back(EngineAsset::MeshMaterial);
//             continue;
//         }
//
//         auto material = std::make_unique<Material>(EngineAsset::MeshShader);
//
//         const String sourceName(sourceMaterial->name.data, sourceMaterial->name.length);
//
//         material->SetName(sourceName.empty()
//                               ? metadata.sourcePath.GetStem() + "_Material_" + std::to_string(i)
//                               : sourceName);
//         material->SetHandle(AssetHandle::Generate());
//
//         const ufbx_material_map* diffuseMap = nullptr;
//
//         if (sourceMaterial->pbr.base_color.texture &&
//             sourceMaterial->pbr.base_color.texture_enabled)
//         {
//             diffuseMap = &sourceMaterial->pbr.base_color;
//         }
//         else if (sourceMaterial->fbx.diffuse_color.texture &&
//                  sourceMaterial->fbx.diffuse_color.texture_enabled)
//         {
//             diffuseMap = &sourceMaterial->fbx.diffuse_color;
//         }
//
//         if (diffuseMap)
//         {
//             const ufbx_texture* sourceTexture = diffuseMap->texture;
//
//             const String relativeTexturePath(
//                 sourceTexture->relative_filename.data,
//                 sourceTexture->relative_filename.length);
//
//             if (!relativeTexturePath.empty())
//             {
//                 const VirtualPath diffuseTexturePath = directory.Join(relativeTexturePath);
//
//                 const VirtualPath diffuseTextureMetadataPath = assetSystem.GetMetadataPath(diffuseTexturePath);
//
//                 const String metadataText = fileSystem.ReadText(diffuseTextureMetadataPath);
//
//                 if (!metadataText.empty())
//                 {
//                     AssetMetadata textureMetadata = {};
//                     textureMetadata.Deserialize(YAML::Load(metadataText));
//
//                     material->AddParameter(
//                         "diffuseColorTexture",
//                         MaterialParameterType::Texture2D,
//                         textureMetadata.handle);
//                 }
//             }
//         }
//
//         const AssetHandle handle = material->GetHandle();
//         assetDatabase.Add(std::move(material));
//
//         materialHandles.push_back(handle);
//     }
//
//     return materialHandles;
// }

namespace
{
Matrix ToMatrix(const ufbx_matrix& source)
{
    return Matrix(
        static_cast<float>(source.m00),
        static_cast<float>(source.m10),
        static_cast<float>(source.m20),
        0.0f,

        static_cast<float>(source.m01),
        static_cast<float>(source.m11),
        static_cast<float>(source.m21),
        0.0f,

        static_cast<float>(source.m02),
        static_cast<float>(source.m12),
        static_cast<float>(source.m22),
        0.0f,

        static_cast<float>(source.m03),
        static_cast<float>(source.m13),
        static_cast<float>(source.m23),
        1.0f);
}
} // namespace

} // namespace URay
