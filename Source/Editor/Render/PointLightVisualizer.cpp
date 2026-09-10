#include "PointLightVisualizer.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Component/Render/Light/PointLightComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Unit.h"

#include "Render/Scene/Object/Drawable/BillboardObject.h"
#include "Render/Scene/RenderScene.h"

namespace URay
{

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

void PointLightVisualizer::OnAdded(
    EditorVisualContext& context,
    Scene& scene,
    Unit& unit,
    Component& component)
{
    if (visuals.contains(&component))
        return;

    std::unique_ptr<Render::BillboardObject> billboard =
        std::make_unique<Render::BillboardObject>(MakeBillboardState(context, unit, component));

    PointLightVisual visual = {};
    visual.billboard = billboard.get();

    context.renderScene.Add(std::move(billboard));
    visuals.emplace(&component, visual);
}

void PointLightVisualizer::OnRemoved(
    EditorVisualContext& context,
    Scene& scene,
    Unit& unit,
    Component& component)
{
    const auto it = visuals.find(&component);
    if (it == visuals.end())
        return;

    PointLightVisual& visual = it->second;

    if (visual.billboard)
        context.renderScene.Destroy(visual.billboard);

    visuals.erase(it);
}

void PointLightVisualizer::OnUnitWorldTransformUpdated(
    EditorVisualContext& context,
    Scene& scene,
    Unit& unit,
    Component& component)
{
    const auto it = visuals.find(&component);
    if (it == visuals.end())
        return;

    PointLightVisual& visual = it->second;

    if (visual.billboard)
        visual.billboard->Update(MakeBillboardState(context, unit, component));
}

void PointLightVisualizer::OnPropertyChanged(EditorVisualContext& context, Scene&, Unit& unit, Component& component, const Property& property)
{
    if (property.name != "Color")
        return;

    const auto it = visuals.find(&component);
    if (it == visuals.end() || !it->second.billboard)
        return;

    it->second.billboard->Update(MakeBillboardState(context, unit, component));
}

} // namespace URay
