#pragma once

#include "Editor/Render/EditorComponentVisualizer.h"

#include <unordered_map>

namespace URay
{

class SelectionSystem;

namespace Render
{
class LineObject;
struct LineObjectState;
} // namespace Render

class DecalVisualizer final : public EditorComponentVisualizer
{
public:
    DecalVisualizer(Engine& engine, SelectionSystem& selectionSystem);
    ~DecalVisualizer() override;

public:
    void OnAdded(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) override;
    void OnRemoved(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) override;
    void OnUnitWorldTransformUpdated(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) override;
    void OnPropertyChanged(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component, const Property& property) override;

private:
    struct DecalVisual
    {
        Unit* unit = nullptr;
        Render::LineObject* line = nullptr;
    };

    static Render::LineObjectState MakeLineState(Unit& unit, Component& component);

    void OnSelectionChanged(Unit* previousUnit, Unit* selectedUnit);
    void CreateLine(EditorVisualContext& context, Component& component, DecalVisual& visual);
    void DestroyLine(EditorVisualContext& context, DecalVisual& visual);

private:
    Engine& engine;
    SelectionSystem& selectionSystem;
    std::unordered_map<Component*, DecalVisual> visuals;
};

} // namespace URay
