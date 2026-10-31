#include "MeshPreviewSession.h"

#include <Core/Math/Math.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/Mesh/Mesh.h>
#include <Render/RHI/RenderTarget.h>
#include <Render/Rendering/Scene/SceneSystem.h>

#include <algorithm>
#include <cmath>

namespace URay
{

MeshPreviewSession::MeshPreviewSession(
    SceneSystem& sceneSystem,
    AssetDatabase& assetDatabase,
    Render::SceneSystem& renderSceneSystem,
    Render::Device& device)
    : assetDatabase(assetDatabase),
      renderSceneSystem(renderSceneSystem),
      previewScene(sceneSystem, assetDatabase),
      renderTargets(device)
{
}

bool MeshPreviewSession::Initialize(
    const AssetHandle& newMeshHandle,
    const Extent2D& newExtent)
{
    if (initialized || newExtent.width == 0 || newExtent.height == 0)
        return false;

    Mesh* mesh = assetDatabase.Find<Mesh>(newMeshHandle);
    if (!mesh)
        return false;

    if (!previewScene.Initialize(newMeshHandle))
        return false;

    if (!renderTargets.Initialize(newExtent))
    {
        previewScene.Finalize();
        return false;
    }

    const AABB& bounds = mesh->GetLocalBounds();
    const Vector3 center = bounds.GetCenter();
    const float radius = std::max(bounds.GetExtent().GetLength(), 0.1f);

    EditorView& view = camera.GetView();
    const float aspect =
        static_cast<float>(newExtent.width) / static_cast<float>(newExtent.height);
    const float halfVerticalFov = Math::DegToRad(view.fov) * 0.5f;
    const float halfHorizontalFov = std::atan(std::tan(halfVerticalFov) * aspect);
    const float limitingHalfFov = std::min(halfVerticalFov, halfHorizontalFov);
    const float distance = radius / std::sin(limitingHalfFov) * 1.2f;

    view.position = center - view.GetForward() * distance;
    view.farPlane = std::max(view.farPlane, distance + radius * 2.0f);

    meshHandle = newMeshHandle;
    extent = newExtent;
    initialized = true;

    return true;
}

void MeshPreviewSession::Finalize()
{
    if (!initialized)
        return;

    renderTargets.Finalize();
    previewScene.Finalize();

    extent = {};
    initialized = false;
}

std::optional<Render::SceneRenderInfo> MeshPreviewSession::BuildSceneRenderInfo() const
{
    if (!initialized)
        return std::nullopt;

    Scene* scene = previewScene.GetScene();
    if (!scene)
        return std::nullopt;

    Render::RenderScene* renderScene = renderSceneSystem.GetRenderScene(scene);
    if (!renderScene)
        return std::nullopt;

    const EditorView& editorView = camera.GetView();

    Render::SceneRenderInfo info = {};
    info.scenes.push_back(renderScene);
    info.targets = renderTargets.GetTargets();
    info.view = {
        .cameraPosition = editorView.position,
        .cameraDirection = editorView.GetForward(),
        .nearPlane = editorView.nearPlane,
        .farPlane = editorView.farPlane,
        .viewMatrix = editorView.GetViewMatrix(),
        .projMatrix = editorView.GetProjMatrix(extent),
        .viewMode = Render::ViewMode::Lit
    };

    return info;
}

Render::TextureView* MeshPreviewSession::GetDisplayView() const
{
    if (!initialized)
        return nullptr;

    const Render::SceneRenderTargets targets = renderTargets.GetTargets();
    return targets.postProcess ? targets.postProcess->GetColorView(0) : nullptr;
}

} // namespace URay
