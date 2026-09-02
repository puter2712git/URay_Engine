#include "DecalComponent.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Scene/Unit.h"

#include "Render/Scene/Object/DecalObject.h"

namespace URay
{

URAY_REGISTER_CLASS(DecalComponent)
URAY_REGISTER_COMPONENT(DecalComponent)

DecalComponent::DecalComponent() = default;

DecalComponent::~DecalComponent() = default;

void DecalComponent::RegisterClass()
{
    Super::RegisterClass();

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
    state.localBounds = AABB{
        .min = Vector3(-extent.x, -extent.y, -extent.z),
        .max = Vector3(extent.x, extent.y, extent.z)
    };

    renderObject = new Render::DecalObject(state);
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
    state.localBounds = AABB{
        .min = Vector3(-extent.x, -extent.y, -extent.z),
        .max = Vector3(extent.x, extent.y, extent.z)
    };
    state.material = material;

    Render::DecalObject* decalObject =
        static_cast<Render::DecalObject*>(renderObject);
    decalObject->Update(state);
}

} // namespace URay
