#include "AmbientLightComponent.h"

#include "Render/Scene/Object/Light/AmbientLightObject.h"

namespace URay
{

URAY_REGISTER_CLASS(AmbientLightComponent)
URAY_REGISTER_COMPONENT(AmbientLightComponent)

void AmbientLightComponent::RegisterClass()
{
    Super::RegisterClass();
}

AmbientLightComponent::AmbientLightComponent() = default;

AmbientLightComponent::~AmbientLightComponent() = default;

Render::RenderObject* AmbientLightComponent::CreateRenderObject()
{
    Unit* unit = GetOwner();
    if (!unit)
        return nullptr;

    Render::AmbientLightObjectState state = {};
    state.intensity = intensity;
    state.color = color;

    renderObject = new Render::AmbientLightObject(state);
    return renderObject;
}

void AmbientLightComponent::UpdateRenderObject()
{
    Unit* unit = GetOwner();
    if (!unit)
        return;

    Render::AmbientLightObjectState state = {};
    state.intensity = intensity;
    state.color = color;

    Render::AmbientLightObject* object =
        static_cast<Render::AmbientLightObject*>(renderObject);
    object->Update(state);
}

} // namespace URay
