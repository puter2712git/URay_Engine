#pragma once

#include "Engine/Ray/EventRay.h"

#include <unordered_map>
#include <vector>

namespace URay
{

class Engine;
class Unit;
class Scene;

namespace Render
{
class RenderObject;
}

class EditorSceneRenderer
{
public:
    EditorSceneRenderer(Engine& engine);
    ~EditorSceneRenderer();

public:
    bool Initialize();
    void Finalize();

private:
    void OnUnitAdded(Scene* scene, Unit* unit);
    void OnUnitRemoved(Scene* scene, Unit* unit);
    void OnUnitTransformUpdated(Scene* scene, Unit* unit);

private:
    Engine& engine;

    std::unordered_map<Unit*, std::vector<Render::RenderObject*>> renderObjects;

    RayHandle unitAddHandle = 0;
    RayHandle unitRemoveHandle = 0;
    RayHandle unitTransformUpdateHandle = 0;
};

} // namespace URay
