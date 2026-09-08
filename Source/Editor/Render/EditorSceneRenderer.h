#pragma once

#include "Editor/Render/EditorVisualizerRegistry.h"
#include "Engine/Ray/EventRay.h"

namespace URay
{

class Engine;
class Component;
class Property;
class Unit;
class Scene;

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
    void OnComponentPropertyChanged(Scene* scene, Unit* unit, Component* component, const Property& property);

private:
    Engine& engine;

    EditorVisualizerRegistry visualizerRegistry;

    RayHandle unitAddHandle = 0;
    RayHandle unitRemoveHandle = 0;
    RayHandle unitTransformUpdateHandle = 0;
    RayHandle componentPropertyChangedHandle = 0;
};

} // namespace URay
