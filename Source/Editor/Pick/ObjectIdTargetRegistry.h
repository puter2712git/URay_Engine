#pragma once

#include <Core/Type/Types.h>

#include <unordered_map>

namespace URay::Render
{
class RenderObject;
class RenderScene;
class SceneSystem;
} // namespace URay::Render

namespace URay
{

class Component;
class Unit;

class ObjectIdTargetRegistry
{
public:
    explicit ObjectIdTargetRegistry(Render::SceneSystem& sceneSystem);
    ~ObjectIdTargetRegistry();

public:
    Unit* FindUnit(uint32 objectId) const;

private:
    void OnRenderObjectAdded(
        Render::RenderScene* scene,
        Render::RenderObject* object,
        Unit* unit,
        Component* component);
    void OnRenderObjectDestroyed(
        Render::RenderScene* scene,
        Render::RenderObject* object);

private:
    Render::SceneSystem& sceneSystem;
    std::unordered_map<uint32, Unit*> objectTargets;
};

} // namespace URay
