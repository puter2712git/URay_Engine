#pragma once

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

class SceneSystem
{
public:
    SceneSystem();
    ~SceneSystem();

public:
    bool Initialize();
    void Finalize();

    RenderScene* GetRenderScene(Scene* scene) const;

private:
    void OnEngineSceneAdded(Scene* scene);
    void OnEngineSceneDestroyed(Scene* scene);

    void OnEngineSceneComponentAdded(Scene* scene, Unit* unit, Component* component);
    void OnEngineSceneComponentDestroyed(Scene* scene, Unit* unit, Component* component);

private:
    std::unordered_map<Scene*, std::unique_ptr<RenderScene>> scenes;
};

} // namespace URay::Render
