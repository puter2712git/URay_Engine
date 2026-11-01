#pragma once

#include <Core/Type/TypeAlias.h>
#include <Editor/Visual/Visualizer/EditorVisualizerRegistry.h>

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
    void OnComponentPropertyChanged(Scene* scene, Unit* unit, Component* component, const Property* property);

private:
    Engine& engine;
    SelectionSystem& selectionSystem;

    EditorVisualizerRegistry visualizerRegistry;
    HashMap<Component*, UniquePtr<EditorComponentVisualizer>> visualizers;
};

} // namespace URay
