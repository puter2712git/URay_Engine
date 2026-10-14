#pragma once

#include <Editor/Visual/Visualizer/EditorComponentVisualizer.h>

namespace URay
{

class SelectionSystem;

namespace Render
{
class LineObject;
struct LineObjectState;
class RenderScene;
} // namespace Render

class CameraVisualizer final : public EditorComponentVisualizer
{
public:
    CameraVisualizer(EditorVisualContext& context, Unit& unit, Component& component, SelectionSystem& selectionSystem);
    ~CameraVisualizer() override;

public:
    void OnUnitWorldTransformUpdated(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) override;
    void OnPropertyChanged(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component, const Property& property) override;

private:
    struct CameraVisual
    {
        Unit* unit = nullptr;
        Component* component = nullptr;
        Render::LineObject* line = nullptr;
    };

    static Render::LineObjectState MakeLineState(Component& component);

    void OnSelectionChanged(Unit* previousUnit, Unit* selectedUnit);
    void CreateLine(EditorVisualContext& context, Component& component, CameraVisual& visual);
    void DestroyLine(EditorVisualContext& context, CameraVisual& visual);

private:
    Engine& engine;
    Render::RenderScene& renderScene;
    SelectionSystem& selectionSystem;
    CameraVisual visual;
};

} // namespace URay
