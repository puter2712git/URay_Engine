#pragma once

#include "Engine/Ray/EventRay.h"

#include "Core/Type/Types.h"

#include <memory>
#include <unordered_map>

namespace URay
{
class Scene;
class Unit;
class Component;
} // namespace URay

namespace URay::Render
{

class RenderScene;
class RenderObject;

class SceneSystem
{
public:
    SceneSystem();
    ~SceneSystem();

public:
    bool Initialize();
    void Finalize();

    RenderObject* AddObject(RenderScene& scene, std::unique_ptr<RenderObject> object, Component* component);
    void DestroyObject(RenderScene& scene, RenderObject* object);

    uint32 AllocateObjectId();

    RenderScene* GetRenderScene(Scene* scene) const;

    EventRay<RenderScene*, RenderObject*, Unit*, Component*>& GetObjectAddRay() { return objectAddRay; }
    EventRay<RenderScene*, RenderObject*>& GetObjectDestroyRay() { return objectDestroyRay; }
    EventRay<RenderScene*, RenderObject*>& GetObjectUpdateRay() { return objectUpdateRay; }

private:
    void OnEngineSceneAdded(Scene* scene);
    void OnEngineSceneDestroyed(Scene* scene);

    void OnEngineSceneComponentAdded(Scene* scene, Unit* unit, Component* component);
    void OnEngineSceneComponentDestroyed(Scene* scene, Unit* unit, Component* component);

private:
    std::unordered_map<Scene*, std::unique_ptr<RenderScene>> scenes;

    EventRay<RenderScene*, RenderObject*, Unit*, Component*> objectAddRay;
    EventRay<RenderScene*, RenderObject*> objectDestroyRay;
    EventRay<RenderScene*, RenderObject*> objectUpdateRay;

    uint32 nextObjectId = 1;
};

} // namespace URay::Render
