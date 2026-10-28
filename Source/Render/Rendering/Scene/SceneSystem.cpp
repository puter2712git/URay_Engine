#include "SceneSystem.h"

#include <Render/Rendering/Object/RenderObject.h>
#include <Render/Rendering/Scene/RenderScene.h>

#include <Engine/Component/Component.h>
#include <Engine/Component/Render/RenderComponent.h>
#include <Engine/Scene/Unit.h>
#include <Engine/Engine.h>
#include <Engine/Scene/SceneSystem.h>

#include <cassert>

namespace URay::Render
{

SceneSystem::SceneSystem()
{
    URAY_DECLARE_RAY(ObjectAdded, RenderScene*, RenderObject*, Unit*, Component*);
    URAY_DECLARE_RAY(ObjectDestroyed, RenderScene*, RenderObject*);
    URAY_DECLARE_RAY(ObjectUpdated, RenderScene*, RenderObject*);
}

SceneSystem::~SceneSystem() = default;

bool SceneSystem::Initialize()
{
    URay::SceneSystem& sceneSystem = gEngine->GetSceneSystem();

    sceneSystem.Register("SceneAdded", this, &SceneSystem::OnEngineSceneAdded);
    sceneSystem.Register("SceneDestroyed", this, &SceneSystem::OnEngineSceneDestroyed);
    sceneSystem.Register("ComponentAdded", this, &SceneSystem::OnEngineSceneComponentAdded);
    sceneSystem.Register("ComponentDestroyed", this, &SceneSystem::OnEngineSceneComponentDestroyed);

    return true;
}

void SceneSystem::Finalize()
{
    URay::SceneSystem& sceneSystem = gEngine->GetSceneSystem();

    sceneSystem.UnregisterAll(this);
}

RenderObject* SceneSystem::AddObject(RenderScene& scene, std::unique_ptr<RenderObject> object, Component* component)
{
    if (!object)
        return nullptr;

    RenderObject* objectPtr = object.get();
    objectPtr->SetId(AllocateObjectId());

    Unit* unit = component ? component->GetOwner() : nullptr;
    scene.Add(std::move(object), unit);

    Emit("ObjectAdded", &scene, objectPtr, unit, component);

    return objectPtr;
}

void SceneSystem::DestroyObject(RenderScene& scene, RenderObject* object)
{
    if (!object)
        return;

    Emit("ObjectDestroyed", &scene, object);
    scene.Destroy(object);
}

uint32 SceneSystem::AllocateObjectId()
{
    assert(nextObjectId != 0);
    return nextObjectId++;
}

RenderScene* SceneSystem::GetRenderScene(Scene* scene) const
{
    const auto it = scenes.find(scene);
    if (it == scenes.end())
        return nullptr;

    return it->second.get();
}

void SceneSystem::OnEngineSceneAdded(Scene* scene)
{
    const auto it = scenes.find(scene);
    assert(it == scenes.end());

    std::unique_ptr<RenderScene> newRenderScene = std::make_unique<RenderScene>(*this);
    scenes.insert({ scene, std::move(newRenderScene) });
}

void SceneSystem::OnEngineSceneDestroyed(Scene* scene)
{
    scenes.erase(scene);
}

void SceneSystem::OnEngineSceneComponentAdded(Scene* scene, Unit* unit, Component* component)
{
    const auto it = scenes.find(scene);
    if (it == scenes.end())
        return;

    auto& renderScene = it->second;

    RenderComponent* renderComponent = Cast<RenderComponent>(component);
    if (!renderComponent)
        return;

    std::unique_ptr<RenderObject> newRenderObject(renderComponent->CreateRenderObject());

    AddObject(*renderScene, std::move(newRenderObject), component);
}

void SceneSystem::OnEngineSceneComponentDestroyed(Scene* scene, Unit* unit, Component* component)
{
    const auto it = scenes.find(scene);
    assert(it != scenes.end());

    auto& renderScene = it->second;

    RenderComponent* renderComponent = Cast<RenderComponent>(component);
    if (!renderComponent)
        return;

    RenderObject* renderObject = renderComponent->GetRenderObject();
    DestroyObject(*renderScene, renderObject);
}

} // namespace URay::Render
