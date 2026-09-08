#include "EditorSceneRenderer.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Component/Component.h"
#include "Engine/Component/Render/Light/DirectionalLightComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

#include "Core/Log/Log.h"

#include "Render/Scene/Object/Drawable/MeshObject.h"
#include "Render/Scene/RenderScene.h"

namespace URay
{

EditorSceneRenderer::EditorSceneRenderer(Engine& engine)
    : engine(engine) {}

EditorSceneRenderer::~EditorSceneRenderer() = default;

bool EditorSceneRenderer::Initialize()
{
    SceneSystem& sceneSystem = engine.GetSceneSystem();
    unitAddHandle = sceneSystem.RegisterUnitAddCallback([this](Scene* scene, Unit* unit)
                                                        { OnUnitAdded(scene, unit); });
    unitRemoveHandle = sceneSystem.RegisterUnitRemoveCallback([this](Scene* scene, Unit* unit)
                                                              { OnUnitRemoved(scene, unit); });
    unitTransformUpdateHandle = sceneSystem.RegisterUnitWorldTransformUpdateCallback([this](Scene* scene, Unit* unit)
                                                                                     { OnUnitTransformUpdated(scene, unit); });

    return true;
}

void EditorSceneRenderer::Finalize()
{
    SceneSystem& sceneSystem = engine.GetSceneSystem();

    sceneSystem.UnregisterUnitWorldTransformUpdateCallback(unitTransformUpdateHandle);
    sceneSystem.UnregisterUnitRemoveCallback(unitRemoveHandle);
    sceneSystem.UnregisterUnitAddCallback(unitAddHandle);
}

void EditorSceneRenderer::OnUnitAdded(Scene* scene, Unit* unit)
{
    Logger::Log("Unit added: " + unit->GetName());

    Scene* editorScene = engine.GetSceneSystem().GetSceneByType(SceneType::Editor);

    std::vector<Component*> components = unit->GetComponents();
    for (Component* comp : components)
    {
        if (DirectionalLightComponent* directionalLight = Cast<DirectionalLightComponent>(comp))
        {
            Render::MeshObjectState state = {
                .worldMatrix = unit->GetTransform() ? unit->GetTransform()->GetWorldMatrix() : Matrix::Identity,
                .mesh = engine.GetAssetSystem().GetDefaultAssets().arrowMesh,
                .materials = { engine.GetAssetSystem().GetDefaultAssets().meshMaterial }
            };
            std::unique_ptr<Render::MeshObject> meshObject = std::make_unique<Render::MeshObject>(state);

            renderObjects[unit].push_back(meshObject.get());

            editorScene->GetRenderScene()->Add(std::move(meshObject));
        }
    }
}

void EditorSceneRenderer::OnUnitRemoved(Scene* scene, Unit* unit)
{
    Logger::Log("Unit removed: " + unit->GetName());

    Scene* editorScene = engine.GetSceneSystem().GetSceneByType(SceneType::Editor);

    for (Render::RenderObject* obj : renderObjects[unit])
    {
        if (obj)
        {
            editorScene->GetRenderScene()->Destroy(obj);
            obj = nullptr;
        }
    }

    renderObjects.erase(unit);
}

void EditorSceneRenderer::OnUnitTransformUpdated(Scene* scene, Unit* unit)
{
    Scene* editorScene = engine.GetSceneSystem().GetSceneByType(SceneType::Editor);

    for (Render::RenderObject* obj : renderObjects[unit])
    {
        if (obj)
        {
            editorScene->GetRenderScene()->Destroy(obj);
            obj = nullptr;
        }
    }

    renderObjects.erase(unit);

    std::vector<Component*> components = unit->GetComponents();
    for (Component* comp : components)
    {
        if (DirectionalLightComponent* directionalLight = Cast<DirectionalLightComponent>(comp))
        {
            Render::MeshObjectState state = {
                .worldMatrix = unit->GetTransform() ? unit->GetTransform()->GetWorldMatrix() : Matrix::Identity,
                .mesh = engine.GetAssetSystem().GetDefaultAssets().arrowMesh,
                .materials = { engine.GetAssetSystem().GetDefaultAssets().meshMaterial }
            };
            std::unique_ptr<Render::MeshObject> meshObject = std::make_unique<Render::MeshObject>(state);

            renderObjects[unit].push_back(meshObject.get());

            editorScene->GetRenderScene()->Add(std::move(meshObject));
        }
    }
}

} // namespace URay
