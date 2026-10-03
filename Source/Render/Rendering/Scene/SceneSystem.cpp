#include "SceneSystem.h"

#include "Render/Rendering/Object/RenderObject.h"
#include "Render/Rendering/Scene/RenderScene.h"

#include "Engine/Component/Render/RenderComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/SceneSystem.h"

#include <cassert>

namespace URay::Render
{

SceneSystem::SceneSystem() = default;

SceneSystem::~SceneSystem() = default;

bool SceneSystem::Initialize()
{
    URay::SceneSystem& sceneSystem = gEngine->GetSceneSystem();

    sceneSystem.GetSceneAddRay().Register(this, [this](Scene* scene)
                                          { OnEngineSceneAdded(scene); });
    sceneSystem.GetSceneDestroyRay().Register(this, [this](Scene* scene)
                                              { OnEngineSceneDestroyed(scene); });
    sceneSystem.GetComponentAddRay().Register(this, [this](Scene* scene, Unit* unit, Component* component)
                                              { OnEngineSceneComponentAdded(scene, unit, component); });
    sceneSystem.GetComponentDestroyRay().Register(this, [this](Scene* scene, Unit* unit, Component* component)
                                                  { OnEngineSceneComponentDestroyed(scene, unit, component); });

    return true;
}

void SceneSystem::Finalize()
{
    URay::SceneSystem& sceneSystem = gEngine->GetSceneSystem();

    sceneSystem.GetComponentDestroyRay().UnregisterAll(this);
    sceneSystem.GetComponentAddRay().UnregisterAll(this);
    sceneSystem.GetSceneDestroyRay().UnregisterAll(this);
    sceneSystem.GetSceneAddRay().UnregisterAll(this);
}

RenderObject* SceneSystem::AddObject(RenderScene& scene, std::unique_ptr<RenderObject> object, Component* component)
{
    if (!object)
        return nullptr;

    RenderObject* objectPtr = object.get();
    objectPtr->SetId(AllocateObjectId());

    scene.Add(std::move(object));

    Unit* unit = component ? component->GetOwner() : nullptr;
    objectAddRay.Emit(&scene, objectPtr, unit, component);

    return objectPtr;
}

void SceneSystem::DestroyObject(RenderScene& scene, RenderObject* object)
{
    if (!object)
        return;

    objectDestroyRay.Emit(&scene, object);
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