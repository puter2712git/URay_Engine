#include "DirectionalLightComponent.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Scene/Unit.h"

#include "Render/Scene/Object/Light/DirectionalLightObject.h"

namespace URay
{

URAY_REGISTER_CLASS(DirectionalLightComponent)
URAY_REGISTER_COMPONENT(DirectionalLightComponent)

void DirectionalLightComponent::RegisterClass()
{
    Super::RegisterClass();
}

DirectionalLightComponent::DirectionalLightComponent() = default;

DirectionalLightComponent::~DirectionalLightComponent() = default;

Render::RenderObject* DirectionalLightComponent::CreateRenderObject()
{
    Unit* unit = GetOwner();
    if (!unit)
        return nullptr;

    TransformComponent* transform = unit->GetTransform();

    Render::DirectionalLightObjectState state = {};
    state.direction = transform ? transform->GetForward() : Vector3::Forward;
    state.intensity = intensity;
    state.color = color;

    renderObject = new Render::DirectionalLightObject(state);
    return renderObject;
}

void DirectionalLightComponent::UpdateRenderObject()
{
    Unit* unit = GetOwner();
    if (!unit)
        return;

    TransformComponent* transform = unit->GetTransform();

    Render::DirectionalLightObjectState state = {};
    state.direction = transform ? transform->GetForward() : Vector3::Forward;
    state.intensity = intensity;
    state.color = color;

    Render::DirectionalLightObject* object =
        static_cast<Render::DirectionalLightObject*>(renderObject);
    object->Update(state);
}

} // namespace URay
