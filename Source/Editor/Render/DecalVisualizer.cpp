#include "DecalVisualizer.h"

#include "Editor/Selection/SelectionSystem.h"

#include "Engine/Component/Render/DecalComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Unit.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Scene.h"

#include "Render/Scene/Object/Drawable/LineObject.h"
#include "Render/Scene/RenderScene.h"

namespace URay
{

DecalVisualizer::DecalVisualizer(Engine& engine, SelectionSystem& selectionSystem)
    : engine(engine), selectionSystem(selectionSystem)
{
    selectionSystem.GetOnSelectionChangedRay().Register(
        this,
        [this](Unit* previousUnit, Unit* selectedUnit)
        { OnSelectionChanged(previousUnit, selectedUnit); });
}

DecalVisualizer::~DecalVisualizer()
{
    selectionSystem.GetOnSelectionChangedRay().UnregisterAll(this);
}

Render::LineObjectState DecalVisualizer::MakeLineState(Unit& unit, Component& component)
{
    TransformComponent* transform = unit.GetTransform();
    DecalComponent& decal = static_cast<DecalComponent&>(component);

    const Matrix worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    const Vector3 extent = decal.GetExtent();
    const Vector3 corners[8] = {
        { -extent.x, -extent.y, -extent.z },
        { extent.x, -extent.y, -extent.z },
        { extent.x, extent.y, -extent.z },
        { -extent.x, extent.y, -extent.z },
        { -extent.x, -extent.y, extent.z },
        { extent.x, -extent.y, extent.z },
        { extent.x, extent.y, extent.z },
        { -extent.x, extent.y, extent.z }
    };
    constexpr uint32 edges[12][2] = {
        { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 },
        { 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 },
        { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }
    };

    Render::LineObjectState state = {};
    for (const auto& edge : edges)
    {
        state.lines.push_back({
            .start = worldMatrix.TransformPoint(corners[edge[0]]),
            .end = worldMatrix.TransformPoint(corners[edge[1]]),
            .color = Color::Yellow });
    }

    return state;
}

void DecalVisualizer::OnAdded(EditorVisualContext& context, Scene&, Unit& unit, Component& component)
{
    auto [it, added] = visuals.emplace(&component, DecalVisual{ .unit = &unit });
    if (!added || selectionSystem.GetSelectedUnit() != &unit)
        return;

    CreateLine(context, component, it->second);
}

void DecalVisualizer::OnRemoved(EditorVisualContext& context, Scene&, Unit&, Component& component)
{
    const auto it = visuals.find(&component);
    if (it == visuals.end())
        return;

    DestroyLine(context, it->second);
    visuals.erase(it);
}

void DecalVisualizer::OnUnitWorldTransformUpdated(EditorVisualContext&, Scene&, Unit& unit, Component& component)
{
    const auto it = visuals.find(&component);
    if (it == visuals.end() || !it->second.line)
        return;

    it->second.line->Update(MakeLineState(unit, component));
}

void DecalVisualizer::OnPropertyChanged(EditorVisualContext&, Scene&, Unit& unit, Component& component, const Property& property)
{
    if (property.name != "Extent")
        return;

    const auto it = visuals.find(&component);
    if (it == visuals.end() || !it->second.line)
        return;

    it->second.line->Update(MakeLineState(unit, component));
}

void DecalVisualizer::OnSelectionChanged(Unit* previousUnit, Unit* selectedUnit)
{
    if (previousUnit == selectedUnit)
        return;

    Scene* editorScene = engine.GetSceneSystem().GetSceneByType(SceneType::Editor);
    if (!editorScene)
        return;

    EditorVisualContext context = {
        .engine = engine,
        .renderScene = *editorScene->GetRenderScene()
    };

    for (auto& [component, visual] : visuals)
    {
        if (visual.unit == previousUnit)
            DestroyLine(context, visual);
        if (visual.unit == selectedUnit)
            CreateLine(context, *component, visual);
    }
}

void DecalVisualizer::CreateLine(EditorVisualContext& context, Component& component, DecalVisual& visual)
{
    if (visual.line)
        return;

    std::unique_ptr<Render::LineObject> line =
        std::make_unique<Render::LineObject>(MakeLineState(*visual.unit, component));
    visual.line = line.get();
    context.renderScene.Add(std::move(line));
}

void DecalVisualizer::DestroyLine(EditorVisualContext& context, DecalVisual& visual)
{
    if (!visual.line)
        return;

    context.renderScene.Destroy(visual.line);
    visual.line = nullptr;
}

} // namespace URay
