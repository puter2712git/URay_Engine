#include "DecalComponent.h"

#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/EngineAsset.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Unit.h"

#include "Render/Rendering/Object/DecalObject.h"

#include <cassert>

namespace URay
{

URAY_REGISTER_CLASS(DecalComponent)
URAY_REGISTER_COMPONENT(DecalComponent)

DecalComponent::DecalComponent()
{
    materialHandle = EngineAsset::DecalMaterial;
}

DecalComponent::~DecalComponent() = default;

void DecalComponent::RegisterClass()
{
    StaticClass()->AddProperty(
        { .type = PropertyType::Vector3,
          .name = "Extent",
          .offset = offsetof(DecalComponent, extent),
          .size = sizeof(Vector3) });
    StaticClass()->AddProperty(
        { .type = PropertyType::Material,
          .name = "Material",
          .offset = offsetof(DecalComponent, materialHandle),
          .size = sizeof(Material*) });
}

Render::RenderObject* DecalComponent::CreateRenderObject()
{
    Unit* unit = GetOwner();
    if (!unit)
        return nullptr;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    TransformComponent* transform = unit->GetTransform();

    Render::DecalObjectState state = {};
    state.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    state.extent = extent;
    state.material = assetDatabase.Find<Material>(materialHandle);

    renderObject = new Render::DecalObject(gEngine->GetRenderSystem(), state);
    return renderObject;
}

void DecalComponent::UpdateRenderObject()
{
    Unit* unit = GetOwner();
    if (!unit)
        return;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    TransformComponent* transform = unit->GetTransform();

    Render::DecalObjectState state = {};
    state.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    state.extent = extent;
    state.material = assetDatabase.Find<Material>(materialHandle);

    Render::DecalObject* decalObject = static_cast<Render::DecalObject*>(renderObject);
    decalObject->Update(state);
}

} // namespace URay
