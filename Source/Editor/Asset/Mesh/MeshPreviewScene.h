#pragma once

#include <Engine/Asset/AssetHandle.h>
#include <Engine/Scene/Scene.h>

namespace URay
{

class AssetDatabase;
class Scene;
class SceneSystem;

class MeshPreviewScene
{
public:
    MeshPreviewScene(SceneSystem& sceneSystem, AssetDatabase& assetDatabase);
    ~MeshPreviewScene();

    MeshPreviewScene(const MeshPreviewScene&) = delete;
    MeshPreviewScene& operator=(const MeshPreviewScene&) = delete;

public:
    bool Initialize(const AssetHandle& meshHandle);
    void Finalize();

    Scene* GetScene() const;

private:
    SceneSystem& sceneSystem;
    AssetDatabase& assetDatabase;

    SceneId sceneId = InvalidSceneId;
};

} // namespace URay
