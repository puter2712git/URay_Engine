#include "DirectionalLightVisualizer.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Unit.h"

#include "Render/Scene/Object/Drawable/MeshObject.h"
#include "Render/Scene/RenderScene.h"

namespace URay
{

namespace
{

Render::MeshObjectState MakeArrowState(EditorVisualContext& context, Unit& unit)
{
    TransformComponent* transform = unit.GetTransform();

    return {
        .worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity,
        .mesh = context.engine.GetAssetSystem().GetDefaultAssets().arrowMesh,
        .materials = { context.engine.GetAssetSystem().GetDefaultAssets().meshMaterial }
    };
}

} // namespace

void DirectionalLightVisualizer::OnAdded(EditorVisualContext& context, Scene&, Unit& unit, Component& component)
{
    if (meshObjects.contains(&component))
        return;

    std::unique_ptr<Render::MeshObject> meshObject = std::make_unique<Render::MeshObject>(MakeArrowState(context, unit));
    meshObjects.emplace(&component, meshObject.get());
    context.renderScene.Add(std::move(meshObject));
}

void DirectionalLightVisualizer::OnRemoved(EditorVisualContext& context, Scene&, Unit&, Component& component)
{
    const auto it = meshObjects.find(&component);
    if (it == meshObjects.end())
        return;

    context.renderScene.Destroy(it->second);
    meshObjects.erase(it);
}

void DirectionalLightVisualizer::OnUnitWorldTransformUpdated(EditorVisualContext& context, Scene&, Unit& unit, Component& component)
{
    const auto it = meshObjects.find(&component);
    if (it == meshObjects.end())
        return;

    it->second->Update(MakeArrowState(context, unit));
}

} // namespace URay
