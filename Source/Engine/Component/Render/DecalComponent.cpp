#include "DecalComponent.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Unit.h"

#include "Render/Scene/Object/DecalObject.h"

#include <cassert>

namespace URay
{

URAY_REGISTER_CLASS(DecalComponent)
URAY_REGISTER_COMPONENT(DecalComponent)

DecalComponent::DecalComponent()
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    material = assetSystem.FindMaterial("Decal");
}

DecalComponent::~DecalComponent() = default;

void DecalComponent::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty(
        { .type = PropertyType::Vector3,
          .name = "Extent",
          .offset = offsetof(DecalComponent, extent),
          .size = sizeof(Vector3),
          .OnChangedCallback = [](Object* owner, const Property&)
          {
              DecalComponent* decalComp = static_cast<DecalComponent*>(owner);
              decalComp->UpdateRenderObject();
          } });
    StaticClass()->AddProperty(
        { .type = PropertyType::Material,
          .name = "Material",
          .offset = offsetof(DecalComponent, material),
          .size = sizeof(Material*),
          .OnChangedCallback = [](Object* owner, const Property&)
          {
              DecalComponent* decalComp = static_cast<DecalComponent*>(owner);
              decalComp->UpdateRenderObject();
          } });
}

Render::RenderObject* DecalComponent::CreateRenderObject()
{
    Unit* unit = GetOwner();
    if (!unit)
        return nullptr;

    TransformComponent* transform = unit->GetTransform();

    Render::DecalObjectState state = {};
    state.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    state.extent = extent;
    state.material = material;

    renderObject = new Render::DecalObject(gEngine->GetRenderSystem(), state); // TODO: Hmm...
    return renderObject;
}

void DecalComponent::UpdateRenderObject()
{
    Unit* unit = GetOwner();
    if (!unit)
        return;

    TransformComponent* transform = unit->GetTransform();

    Render::DecalObjectState state = {};
    state.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    state.extent = extent;
    state.material = material;

    Render::DecalObject* decalObject =
        static_cast<Render::DecalObject*>(renderObject);
    decalObject->Update(state);
}

} // namespace URay
