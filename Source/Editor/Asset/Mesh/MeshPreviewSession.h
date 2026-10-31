#pragma once

#include <Core/Math/Extent2D.h>
#include <Editor/Asset/Mesh/MeshPreviewScene.h>
#include <Editor/Asset/Mesh/PreviewRenderTargets.h>
#include <Editor/EditorCamera.h>
#include <Engine/Asset/AssetHandle.h>
#include <Render/Rendering/RenderPipeline.h>

#include <optional>

namespace URay
{

class AssetDatabase;
class SceneSystem;

namespace Render
{
class Device;
class SceneSystem;
class TextureView;
}

class MeshPreviewSession
{
public:
    MeshPreviewSession(
        SceneSystem& sceneSystem,
        AssetDatabase& assetDatabase,
        Render::SceneSystem& renderSceneSystem,
        Render::Device& device);

    MeshPreviewSession(const MeshPreviewSession&) = delete;
    MeshPreviewSession& operator=(const MeshPreviewSession&) = delete;

public:
    bool Initialize(const AssetHandle& meshHandle, const Extent2D& extent);

    // The caller must ensure GPU use has completed and old UI draw data will not be used.
    // The same requirement applies when the session is destroyed.
    void Finalize();

    std::optional<Render::SceneRenderInfo> BuildSceneRenderInfo() const;
    Render::TextureView* GetDisplayView() const;

    EditorCamera& GetCamera() { return camera; }
    const AssetHandle& GetMeshHandle() const { return meshHandle; }

private:
    AssetDatabase& assetDatabase;
    Render::SceneSystem& renderSceneSystem;

    MeshPreviewScene previewScene;
    PreviewRenderTargets renderTargets;
    EditorCamera camera;

    AssetHandle meshHandle = {};
    Extent2D extent = {};
    bool initialized = false;
};

} // namespace URay
