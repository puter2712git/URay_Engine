#include "DirectionalLightVisualizer.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
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
        .mesh = context.engine.GetAssetSystem().GetDefaultAssets().arrowMesh,
        .materials = { context.engine.GetAssetSystem().GetDefaultAssets().meshMaterial }
    };
}

Render::BillboardObjectState DirectionalLightVisualizer::MakeBillboardState(EditorVisualContext& context, Unit& unit)
{
    TransformComponent* transform = unit.GetTransform();

    return {
        .worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity,
        .mesh = context.engine.GetAssetSystem().GetDefaultAssets().quadMesh,
        .materials = { context.engine.GetAssetSystem().GetDefaultAssets().billboardMaterial }
    };
}

void DirectionalLightVisualizer::OnAdded(EditorVisualContext& context, Scene&, Unit& unit, Component& component)
{
    if (visuals.contains(&component))
        return;

    std::unique_ptr<Render::MeshObject> arrow = std::make_unique<Render::MeshObject>(MakeArrowState(context, unit));
    std::unique_ptr<Render::BillboardObject> billboard = std::make_unique<Render::BillboardObject>(MakeBillboardState(context, unit));

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
        visual.billboard->Update(MakeBillboardState(context, unit));
}

} // namespace URay
