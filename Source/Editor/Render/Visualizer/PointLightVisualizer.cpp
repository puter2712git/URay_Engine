#include "PointLightVisualizer.h"

#include "Editor/Selection/SelectionSystem.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Component/Render/Light/PointLightComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

#include "Core/Math/Math.h"

#include "Render/Scene/Object/Drawable/BillboardObject.h"
#include "Render/Scene/Object/Drawable/LineObject.h"
#include "Render/Scene/RenderScene.h"

namespace URay
{

PointLightVisualizer::PointLightVisualizer(EditorVisualContext& context, Unit& unit, Component& component, SelectionSystem& selectionSystem)
    : engine(context.engine), renderScene(context.renderScene), selectionSystem(selectionSystem)
{
    selectionSystem.GetOnSelectionChangedRay().Register(
        this,
        [this](Unit* previousUnit, Unit* selectedUnit)
        { OnSelectionChanged(previousUnit, selectedUnit); });

    std::unique_ptr<Render::BillboardObject> billboard =
        std::make_unique<Render::BillboardObject>(MakeBillboardState(context, unit, component));
    visual = {
        .unit = &unit,
        .component = &component,
        .billboard = billboard.get()
    };
    renderScene.Add(std::move(billboard));

    if (selectionSystem.GetSelectedUnit() == &unit)
        CreateLine(context, component, visual);
}

PointLightVisualizer::~PointLightVisualizer()
{
    selectionSystem.GetOnSelectionChangedRay().UnregisterAll(this);
    EditorVisualContext context = {
        .engine = engine,
        .renderScene = renderScene
    };
    DestroyLine(context, visual);
    if (visual.billboard)
        renderScene.Destroy(visual.billboard);
}

Render::BillboardObjectState PointLightVisualizer::MakeBillboardState(
    EditorVisualContext& context,
    Unit& unit,
    Component& component)
{
    TransformComponent* transform = unit.GetTransform();
    PointLightComponent& pointLight = static_cast<PointLightComponent&>(component);

    return {
        .worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity,
        .colorTint = pointLight.GetColor(),
        .mesh = context.engine.GetAssetSystem().GetDefaultAssets().quadMesh,
        .materials = { context.engine.GetAssetSystem().GetDefaultAssets().pointLightBillboardMaterial }
    };
}

Render::LineObjectState PointLightVisualizer::MakeLineState(
    EditorVisualContext& context,
    Unit& unit,
    Component& component)
{
    TransformComponent* transform = unit.GetTransform();
    PointLightComponent& pointLight = static_cast<PointLightComponent&>(component);

    const Vector3 center = transform ? transform->GetPosition() : Vector3::Zero;
    const Vector3 axisA = transform ? transform->GetRight() : Vector3::Right;
    const Vector3 axisB = transform ? transform->GetForward() : Vector3::Forward;
    const Vector3 axisC = transform ? transform->GetUp() : Vector3::Up;

    Render::LineObjectState state = {};
    AddCircle(
        state.lines,
        center,
        pointLight.GetRadius(),
        axisA,
        axisB,
        Color::Yellow);
    AddCircle(
        state.lines,
        center,
        pointLight.GetRadius(),
        axisA,
        axisC,
        Color::Yellow);
    AddCircle(
        state.lines,
        center,
        pointLight.GetRadius(),
        axisB,
        axisC,
        Color::Yellow);

    return state;
}

void PointLightVisualizer::AddCircle(
    std::vector<Render::Line>& lines,
    const Vector3& center,
    float radius,
    const Vector3& axisA,
    const Vector3& axisB,
    const Color& color)
{
    constexpr uint32 segments = 32;

    for (uint32 i = 0; i < segments; ++i)
    {
        const float t0 = (Math::TWO_PI * i) / segments;
        const float t1 = (Math::TWO_PI * (i + 1)) / segments;

        const Vector3 p0 = center + radius * (axisA * std::cos(t0) + axisB * std::sin(t0));
        const Vector3 p1 = center + radius * (axisA * std::cos(t1) + axisB * std::sin(t1));

        lines.push_back(Render::Line{
            .start = p0,
            .end = p1,
            .color = color });
    }
}

void PointLightVisualizer::OnUnitWorldTransformUpdated(
    EditorVisualContext& context,
    Scene& scene,
    Unit& unit,
    Component& component)
{
    if (visual.billboard)
        visual.billboard->Update(MakeBillboardState(context, unit, component));
    if (visual.line)
        visual.line->Update(MakeLineState(context, unit, component));
}

void PointLightVisualizer::OnPropertyChanged(EditorVisualContext& context, Scene&, Unit& unit, Component& component, const Property& property)
{
    if (property.name != "Color" && property.name != "Radius")
        return;

    if (visual.billboard)
        visual.billboard->Update(MakeBillboardState(context, unit, component));
    if (visual.line)
        visual.line->Update(MakeLineState(context, unit, component));
}

void PointLightVisualizer::OnSelectionChanged(Unit* previousUnit, Unit* selectedUnit)
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

    if (visual.unit == previousUnit)
        DestroyLine(context, visual);
    if (visual.unit && visual.unit == selectedUnit)
        CreateLine(context, *visual.component, visual);
}

void PointLightVisualizer::CreateLine(EditorVisualContext& context, Component& component, PointLightVisual& visual)
{
    if (visual.line)
        return;

    std::unique_ptr<Render::LineObject> line =
        std::make_unique<Render::LineObject>(MakeLineState(context, *visual.unit, component));
    visual.line = line.get();
    context.renderScene.Add(std::move(line));
}

void PointLightVisualizer::DestroyLine(EditorVisualContext& context, PointLightVisual& visual)
{
    if (!visual.line)
        return;

    context.renderScene.Destroy(visual.line);
    visual.line = nullptr;
}

} // namespace URay
