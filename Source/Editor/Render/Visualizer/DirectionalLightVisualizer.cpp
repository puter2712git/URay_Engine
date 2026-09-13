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

DirectionalLightVisualizer::DirectionalLightVisualizer(EditorVisualContext& context, Unit& unit, Component& component)
    : renderScene(context.renderScene)
{
    std::unique_ptr<Render::MeshObject> arrow = std::make_unique<Render::MeshObject>(MakeArrowState(context, unit));
    std::unique_ptr<Render::BillboardObject> billboard = std::make_unique<Render::BillboardObject>(MakeBillboardState(context, unit, component));

    visual.arrow = arrow.get();
    visual.billboard = billboard.get();

    renderScene.Add(std::move(arrow));
    renderScene.Add(std::move(billboard));
}

DirectionalLightVisualizer::~DirectionalLightVisualizer()
{
    if (visual.arrow)
        renderScene.Destroy(visual.arrow);

    if (visual.billboard)
        renderScene.Destroy(visual.billboard);
}

void DirectionalLightVisualizer::OnUnitWorldTransformUpdated(EditorVisualContext& context, Scene&, Unit& unit, Component& component)
{
    if (visual.arrow)
        visual.arrow->Update(MakeArrowState(context, unit));

    if (visual.billboard)
        visual.billboard->Update(MakeBillboardState(context, unit, component));
}

void DirectionalLightVisualizer::OnPropertyChanged(EditorVisualContext& context, Scene&, Unit& unit, Component& component, const Property& property)
{
    if (property.name != "Color")
        return;

    if (!visual.billboard)
        return;

    visual.billboard->Update(MakeBillboardState(context, unit, component));
}

} // namespace URay
