#pragma once

#include "Editor/Render/EditorComponentVisualizer.h"

#include "Render/Scene/Object/Drawable/LineObject.h"

#include <unordered_map>

namespace URay
{

class SelectionSystem;

namespace Render
{
class LineObject;
struct LineObjectState;
} // namespace Render

class SpotLightVisualizer final : public EditorComponentVisualizer
{
public:
    SpotLightVisualizer(Engine& engine, SelectionSystem& selectionSystem);
    ~SpotLightVisualizer() override;

public:
    void OnAdded(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) override;
    void OnRemoved(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) override;
    void OnUnitWorldTransformUpdated(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) override;
    void OnPropertyChanged(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component, const Property& property) override;

private:
    struct SpotLightVisual
    {
        Unit* unit = nullptr;
        Render::LineObject* line = nullptr;
    };

    static Render::LineObjectState MakeLineState(Unit& unit, Component& component);
    static void AddCone(
        Render::LineObjectState& state,
        const Vector3& origin,
        const Vector3& direction,
        const Vector3& right,
        const Vector3& up,
        float range,
        float angleDegrees,
        const Color& color);

    void OnSelectionChanged(Unit* previousUnit, Unit* selectedUnit);
    void CreateLine(EditorVisualContext& context, Component& component, SpotLightVisual& visual);
    void DestroyLine(EditorVisualContext& context, SpotLightVisual& visual);

private:
    Engine& engine;
    SelectionSystem& selectionSystem;
    std::unordered_map<Component*, SpotLightVisual> visuals;
};

} // namespace URay
