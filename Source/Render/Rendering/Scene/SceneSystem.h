#pragma once

#include <Core/Ray/EventRay.h>

#include <Core/Type/Types.h>

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

private:
    void OnEngineSceneAdded(Scene* scene);
    void OnEngineSceneDestroyed(Scene* scene);

    void OnEngineSceneComponentAdded(Scene* scene, Unit* unit, Component* component);
    void OnEngineSceneComponentDestroyed(Scene* scene, Unit* unit, Component* component);

public:
    EventRay<RenderScene*, RenderObject*, Unit*, Component*> onObjectAdded;
    EventRay<RenderScene*, RenderObject*> onObjectDestroyed;
    EventRay<RenderScene*, RenderObject*> onObjectUpdated;

private:
    std::unordered_map<Scene*, std::unique_ptr<RenderScene>> scenes;

    uint32 nextObjectId = 1;
};

} // namespace URay::Render
