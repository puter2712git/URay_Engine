#include "MeshPreviewScene.h"

#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/Mesh/Mesh.h>
#include <Engine/Component/Render/Light/AmbientLightComponent.h>
#include <Engine/Component/Render/MeshComponent.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Scene/Scene.h>
#include <Engine/Scene/SceneSystem.h>
#include <Engine/Scene/Unit.h>

#include <memory>
#include <utility>

namespace URay
{

MeshPreviewScene::MeshPreviewScene(SceneSystem& sceneSystem, AssetDatabase& assetDatabase)
    : sceneSystem(sceneSystem), assetDatabase(assetDatabase) {}

MeshPreviewScene::~MeshPreviewScene()
{
    Finalize();
}

bool MeshPreviewScene::Initialize(const AssetHandle& meshHandle)
{
    if (sceneId != InvalidSceneId)
        return false;

    Mesh* mesh = assetDatabase.Find<Mesh>(meshHandle);
    if (!mesh)
        return false;

    auto scene = sceneSystem.CreateScene(SceneType::Preview, "");
    Scene* scenePtr = sceneSystem.LoadScene(std::move(scene));

    if (!scenePtr)
        return false;

    sceneId = scenePtr->GetId();

    auto meshUnit = std::make_unique<Unit>();
    meshUnit->SetName(mesh->GetName());
    meshUnit->AddComponent(std::make_unique<TransformComponent>());

    auto meshComponent = std::make_unique<MeshComponent>();
    meshComponent->SetMeshHandle(meshHandle);
    meshUnit->AddComponent(std::move(meshComponent));

    scenePtr->AddUnit(std::move(meshUnit));

    auto lightUnit = std::make_unique<Unit>();
    lightUnit->SetName("Preview Ambient Light");
    lightUnit->AddComponent(std::make_unique<AmbientLightComponent>());

    scenePtr->AddUnit(std::move(lightUnit));

    return true;
}

void MeshPreviewScene::Finalize()
{
    if (sceneId == InvalidSceneId)
        return;

    const SceneId id = sceneId;
    sceneId = InvalidSceneId;

    sceneSystem.UnloadScene(id);
}

Scene* MeshPreviewScene::GetScene() const
{
    if (sceneId == InvalidSceneId)
        return nullptr;

    return sceneSystem.GetScene(sceneId);
}

} // namespace URay
