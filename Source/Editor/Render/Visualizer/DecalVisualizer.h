#pragma once

#include "Editor/Render/Visualizer/EditorComponentVisualizer.h"

namespace URay
{

class SelectionSystem;

namespace Render
{
class LineObject;
struct LineObjectState;
class RenderScene;
} // namespace Render

class DecalVisualizer final : public EditorComponentVisualizer
{
public:
    DecalVisualizer(EditorVisualContext& context, Unit& unit, Component& component, SelectionSystem& selectionSystem);
    ~DecalVisualizer() override;

public:
    void OnUnitWorldTransformUpdated(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) override;
    void OnPropertyChanged(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component, const Property& property) override;

private:
    struct DecalVisual
    {
        Unit* unit = nullptr;
        Component* component = nullptr;
        Render::LineObject* line = nullptr;
    };

    static Render::LineObjectState MakeLineState(Unit& unit, Component& component);

    void OnSelectionChanged(Unit* previousUnit, Unit* selectedUnit);
    void CreateLine(EditorVisualContext& context, Component& component, DecalVisual& visual);
    void DestroyLine(EditorVisualContext& context, DecalVisual& visual);

private:
    Engine& engine;
    Render::RenderScene& renderScene;
    SelectionSystem& selectionSystem;
    DecalVisual visual;
};

} // namespace URay
