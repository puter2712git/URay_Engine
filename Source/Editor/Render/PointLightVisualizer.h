#pragma once

#include "Editor/Render/EditorComponentVisualizer.h"

#include "Render/Scene/Object/Drawable/LineObject.h"

#include <unordered_map>
#include <vector>

namespace URay
{

class SelectionSystem;

namespace Render
{
class BillboardObject;
struct BillboardObjectState;
} // namespace Render

class PointLightVisualizer final : public EditorComponentVisualizer
{
public:
    PointLightVisualizer(Engine& engine, SelectionSystem& selectionSystem);
    ~PointLightVisualizer() override;

public:
    void OnAdded(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) override;
    void OnRemoved(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) override;
    void OnUnitWorldTransformUpdated(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) override;
    void OnPropertyChanged(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component, const Property& property) override;

private:
    struct PointLightVisual
    {
        Unit* unit = nullptr;
        Render::BillboardObject* billboard = nullptr;
        Render::LineObject* line = nullptr;
    };

    static Render::BillboardObjectState MakeBillboardState(EditorVisualContext& context, Unit& unit, Component& component);
    static Render::LineObjectState MakeLineState(EditorVisualContext& context, Unit& unit, Component& component);
    static void AddCircle(std::vector<Render::Line>& lines, const Vector3& center, float radius, const Vector3& axisA, const Vector3& axisB, const Color& color);

    void OnSelectionChanged(Unit* previousUnit, Unit* selectedUnit);
    void CreateLine(EditorVisualContext& context, Component& component, PointLightVisual& visual);
    void DestroyLine(EditorVisualContext& context, PointLightVisual& visual);

private:
    Engine& engine;
    SelectionSystem& selectionSystem;
    std::unordered_map<Component*, PointLightVisual> visuals;
};

} // namespace URay
