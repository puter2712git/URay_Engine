#include "SpotLightComponent.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Scene/Unit.h"

#include "Render/Scene/Object/Light/SpotLightObject.h"

namespace URay
{

URAY_REGISTER_CLASS(SpotLightComponent)
URAY_REGISTER_COMPONENT(SpotLightComponent)

void SpotLightComponent::RegisterClass()
{
    StaticClass()->AddProperty(
        { .type = PropertyType::Float,
          .name = "Range",
          .offset = offsetof(SpotLightComponent, range),
          .size = sizeof(float) });
    StaticClass()->AddProperty(
        { .type = PropertyType::Float,
          .name = "Inner Cone Angle",
          .offset = offsetof(SpotLightComponent, innerConeAngle),
          .size = sizeof(float) });
    StaticClass()->AddProperty(
        { .type = PropertyType::Float,
          .name = "Outer Cone Angle",
          .offset = offsetof(SpotLightComponent, outerConeAngle),
          .size = sizeof(float) });
}

SpotLightComponent::SpotLightComponent() = default;

SpotLightComponent::~SpotLightComponent() = default;

Render::RenderObject* SpotLightComponent::CreateRenderObject()
{
    Unit* unit = GetOwner();
    if (!unit)
        return nullptr;

    TransformComponent* transform = unit->GetTransform();

    Render::SpotLightObjectState state = {};
    state.position = transform ? transform->GetPosition() : Vector3::Zero;
    state.direction = transform ? transform->GetForward() : Vector3::Forward;
    state.range = range;
    state.innerConeAngle = innerConeAngle;
    state.outerConeAngle = outerConeAngle;
    state.intensity = intensity;
    state.color = color;

    renderObject = new Render::SpotLightObject(state);
    return renderObject;
}

void SpotLightComponent::UpdateRenderObject()
{
    Unit* unit = GetOwner();
    if (!unit)
        return;

    TransformComponent* transform = unit->GetTransform();

    Render::SpotLightObjectState state = {};
    state.position = transform ? transform->GetPosition() : Vector3::Zero;
    state.direction = transform ? transform->GetForward() : Vector3::Forward;
    state.range = range;
    state.innerConeAngle = innerConeAngle;
    state.outerConeAngle = outerConeAngle;
    state.intensity = intensity;
    state.color = color;

    Render::SpotLightObject* object =
        static_cast<Render::SpotLightObject*>(renderObject);
    object->Update(state);
}

} // namespace URay
