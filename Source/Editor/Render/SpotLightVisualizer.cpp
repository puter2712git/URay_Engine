#include "SpotLightVisualizer.h"

#include "Editor/Selection/SelectionSystem.h"

#include "Core/Math/Math.h"

#include "Engine/Component/Render/Light/SpotLightComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

#include "Render/Scene/Object/Drawable/LineObject.h"
#include "Render/Scene/RenderScene.h"

#include <cmath>
#include <memory>

namespace URay
{

SpotLightVisualizer::SpotLightVisualizer(Engine& engine, SelectionSystem& selectionSystem)
    : engine(engine), selectionSystem(selectionSystem)
{
    selectionSystem.GetOnSelectionChangedRay().Register(
        this,
        [this](Unit* previousUnit, Unit* selectedUnit)
        { OnSelectionChanged(previousUnit, selectedUnit); });
}

SpotLightVisualizer::~SpotLightVisualizer()
{
    selectionSystem.GetOnSelectionChangedRay().UnregisterAll(this);
}

Render::LineObjectState SpotLightVisualizer::MakeLineState(Unit& unit, Component& component)
{
    TransformComponent* transform = unit.GetTransform();
    SpotLightComponent& spotLight = static_cast<SpotLightComponent&>(component);

    const Vector3 origin = transform ? transform->GetPosition() : Vector3::Zero;
    const Vector3 direction = transform ? transform->GetForward() : Vector3::Forward;
    const Vector3 right = transform ? transform->GetRight() : Vector3::Right;
    const Vector3 up = transform ? transform->GetUp() : Vector3::Up;

    Render::LineObjectState state = {};
    AddCone(
        state,
        origin,
        direction,
        right,
        up,
        spotLight.GetRange(),
        spotLight.GetInnerConeAngle(),
        Color::Blue);
    AddCone(
        state,
        origin,
        direction,
        right,
        up,
        spotLight.GetRange(),
        spotLight.GetOuterConeAngle(),
        Color::Yellow);

    return state;
}

void SpotLightVisualizer::AddCone(
    Render::LineObjectState& state,
    const Vector3& origin,
    const Vector3& direction,
    const Vector3& right,
    const Vector3& up,
    float range,
    float angleDegrees,
    const Color& color)
{
    constexpr uint32 segments = 32;
    constexpr uint32 sideCount = 4;

    const Vector3 baseCenter = origin + direction * range;
    const float radius = range * std::tan(Math::DegToRad(angleDegrees * 0.5f));

    Vector3 points[segments];
    for (uint32 i = 0; i < segments; ++i)
    {
        const float angle = Math::TWO_PI * i / segments;
        points[i] = baseCenter + radius * (right * std::cos(angle) + up * std::sin(angle));
    }

    for (uint32 i = 0; i < segments; ++i)
    {
        state.lines.push_back({
            .start = points[i],
            .end = points[(i + 1) % segments],
            .color = color });
    }

    for (uint32 i = 0; i < sideCount; ++i)
    {
        const uint32 pointIndex = i * segments / sideCount;
        state.lines.push_back({
            .start = origin,
            .end = points[pointIndex],
            .color = color });
    }
}

void SpotLightVisualizer::OnAdded(EditorVisualContext& context, Scene&, Unit& unit, Component& component)
{
    auto [it, added] = visuals.emplace(&component, SpotLightVisual{ .unit = &unit });
    if (!added || selectionSystem.GetSelectedUnit() != &unit)
        return;

    CreateLine(context, component, it->second);
}

void SpotLightVisualizer::OnRemoved(EditorVisualContext& context, Scene&, Unit&, Component& component)
{
    const auto it = visuals.find(&component);
    if (it == visuals.end())
        return;

    DestroyLine(context, it->second);
    visuals.erase(it);
}

void SpotLightVisualizer::OnUnitWorldTransformUpdated(EditorVisualContext&, Scene&, Unit& unit, Component& component)
{
    const auto it = visuals.find(&component);
    if (it == visuals.end() || !it->second.line)
        return;

    it->second.line->Update(MakeLineState(unit, component));
}

void SpotLightVisualizer::OnPropertyChanged(EditorVisualContext&, Scene&, Unit& unit, Component& component, const Property& property)
{
    if (property.name != "Range" && property.name != "Inner Cone Angle" && property.name != "Outer Cone Angle")
        return;

    const auto it = visuals.find(&component);
    if (it == visuals.end() || !it->second.line)
        return;

    it->second.line->Update(MakeLineState(unit, component));
}

void SpotLightVisualizer::OnSelectionChanged(Unit* previousUnit, Unit* selectedUnit)
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

void SpotLightVisualizer::CreateLine(EditorVisualContext& context, Component& component, SpotLightVisual& visual)
{
    if (visual.line)
        return;

    std::unique_ptr<Render::LineObject> line =
        std::make_unique<Render::LineObject>(MakeLineState(*visual.unit, component));
    visual.line = line.get();
    context.renderScene.Add(std::move(line));
}

void SpotLightVisualizer::DestroyLine(EditorVisualContext& context, SpotLightVisual& visual)
{
    if (!visual.line)
        return;

    context.renderScene.Destroy(visual.line);
    visual.line = nullptr;
}

} // namespace URay
