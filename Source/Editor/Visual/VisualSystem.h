#pragma once

#include "Editor/Visual/Visualizer/EditorVisualizerRegistry.h"
#include "Engine/Ray/EventRay.h"

#include <memory>
#include <unordered_map>

namespace URay
{

class Engine;
class Component;
class Property;
class Unit;
class Scene;
class SelectionSystem;
class EditorComponentVisualizer;

class VisualSystem
{
public:
    VisualSystem(Engine& engine, SelectionSystem& selectionSystem);
    ~VisualSystem();

public:
    bool Initialize();
    void Finalize();

private:
    void OnUnitTransformUpdated(Scene* scene, Unit* unit);
    void OnComponentAdded(Scene* scene, Unit* unit, Component* component);
    void OnComponentDestroyed(Scene* scene, Unit* unit, Component* component);
    void OnComponentPropertyChanged(Scene* scene, Unit* unit, Component* component, const Property& property);

private:
    Engine& engine;
    SelectionSystem& selectionSystem;

    EditorVisualizerRegistry visualizerRegistry;
    std::unordered_map<Component*, std::unique_ptr<EditorComponentVisualizer>> visualizers;
};

} // namespace URay
