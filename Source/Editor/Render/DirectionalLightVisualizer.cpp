#include "DirectionalLightVisualizer.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Component/Render/Light/DirectionalLightComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Object/Property/Property.h"
#include "Engine/Scene/Unit.h"

#include "Render/Scene/Object/Drawable/MeshObject.h"
#include "Render/Scene/Object/Drawable/BillboardObject.h"
#include "Render/Scene/RenderScene.h"

namespace URay
{

Render::MeshObjectState DirectionalLightVisualizer::MakeArrowState(EditorVisualContext& context, Unit& unit)
{
    TransformComponent* transform = unit.GetTransform();

    return {
        .worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity,
        .colorTint = Color::Green,
        .mesh = context.engine.GetAssetSystem().GetDefaultAssets().arrowMesh,
        .materials = { context.engine.GetAssetSystem().GetDefaultAssets().meshMaterial }
    };
}

Render::BillboardObjectState DirectionalLightVisualizer::MakeBillboardState(EditorVisualContext& context, Unit& unit, Component& component)
{
    TransformComponent* transform = unit.GetTransform();
    DirectionalLightComponent& directionalLight = static_cast<DirectionalLightComponent&>(component);

    return {
        .worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity,
        .colorTint = directionalLight.GetColor(),
        .mesh = context.engine.GetAssetSystem().GetDefaultAssets().quadMesh,
        .materials = { context.engine.GetAssetSystem().GetDefaultAssets().billboardMaterial }
    };
}

void DirectionalLightVisualizer::OnAdded(EditorVisualContext& context, Scene&, Unit& unit, Component& component)
{
    if (visuals.contains(&component))
        return;

    std::unique_ptr<Render::MeshObject> arrow = std::make_unique<Render::MeshObject>(MakeArrowState(context, unit));
    std::unique_ptr<Render::BillboardObject> billboard = std::make_unique<Render::BillboardObject>(MakeBillboardState(context, unit, component));

    DirectionalLightVisual visual = {};
    visual.arrow = arrow.get();
    visual.billboard = billboard.get();

    context.renderScene.Add(std::move(arrow));
    context.renderScene.Add(std::move(billboard));
    visuals.emplace(&component, visual);
}

void DirectionalLightVisualizer::OnRemoved(EditorVisualContext& context, Scene&, Unit&, Component& component)
{
    const auto it = visuals.find(&component);
    if (it == visuals.end())
        return;

    DirectionalLightVisual& visual = it->second;

    if (visual.arrow)
        context.renderScene.Destroy(visual.arrow);

    if (visual.billboard)
        context.renderScene.Destroy(visual.billboard);

    visuals.erase(it);
}

void DirectionalLightVisualizer::OnUnitWorldTransformUpdated(EditorVisualContext& context, Scene&, Unit& unit, Component& component)
{
    const auto it = visuals.find(&component);
    if (it == visuals.end())
        return;

    DirectionalLightVisual& visual = it->second;

    if (visual.arrow)
        visual.arrow->Update(MakeArrowState(context, unit));

    if (visual.billboard)
        visual.billboard->Update(MakeBillboardState(context, unit, component));
}

void DirectionalLightVisualizer::OnPropertyChanged(EditorVisualContext& context, Scene&, Unit& unit, Component& component, const Property& property)
{
    if (property.name != "Color")
        return;

    const auto it = visuals.find(&component);
    if (it == visuals.end() || !it->second.billboard)
        return;

    it->second.billboard->Update(MakeBillboardState(context, unit, component));
}

} // namespace URay
