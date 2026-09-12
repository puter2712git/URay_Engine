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
};

} // namespace URay
