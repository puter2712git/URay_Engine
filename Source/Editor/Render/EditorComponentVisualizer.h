#pragma once

namespace URay
{

class Component;
class Engine;
class Property;
class Scene;
class Unit;

namespace Render
{
class RenderScene;
}

struct EditorVisualContext
{
    Engine& engine;
    Render::RenderScene& renderScene;
};

class EditorComponentVisualizer
{
public:
    virtual ~EditorComponentVisualizer() = default;

public:
    virtual void OnAdded(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) = 0;
    virtual void OnRemoved(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) = 0;
    virtual void OnUnitWorldTransformUpdated(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) = 0;
    virtual void OnPropertyChanged(EditorVisualContext&, Scene&, Unit&, Component&, const Property&) {}
};

} // namespace URay
