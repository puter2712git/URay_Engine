#pragma once

#include <memory>
#include <unordered_map>

namespace URay
{

class Class;
class Component;
class Engine;
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

class IEditorComponentVisualizer
{
public:
    virtual ~IEditorComponentVisualizer() = default;

public:
    virtual void OnAdded(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) = 0;
    virtual void OnRemoved(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) = 0;
    virtual void OnUnitWorldTransformUpdated(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) = 0;
};

class EditorVisualizerRegistry
{
public:
    template <typename T>
    void Register(std::unique_ptr<IEditorComponentVisualizer> visualizer)
    {
        visualizers.insert_or_assign(T::StaticClass(), std::move(visualizer));
    }

    IEditorComponentVisualizer* Find(Class* cls) const
    {
        const auto it = visualizers.find(cls);
        return it != visualizers.end() ? it->second.get() : nullptr;
    }

private:
    std::unordered_map<Class*, std::unique_ptr<IEditorComponentVisualizer>> visualizers;
};

} // namespace URay
