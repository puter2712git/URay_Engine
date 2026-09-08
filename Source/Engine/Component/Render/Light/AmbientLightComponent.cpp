#include "AmbientLightComponent.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

#include "Render/Scene/Object/Light/AmbientLightObject.h"

namespace URay
{

URAY_REGISTER_CLASS(AmbientLightComponent)
URAY_REGISTER_COMPONENT(AmbientLightComponent)

void AmbientLightComponent::RegisterClass()
{
    Super::RegisterClass();

    const auto onPropertyChanged = [](Object* owner, const Property& property)
    {
        AmbientLightComponent* comp = static_cast<AmbientLightComponent*>(owner);
        comp->UpdateRenderObject();

        Unit* unit = comp->GetOwner();
        Scene* scene = unit ? unit->GetOwner() : nullptr;

        if (scene)
        {
            scene->GetSceneSystem().EmitComponentPropertyChangedRay(
                scene,
                unit,
                comp,
                property);
        }
    };

    StaticClass()->AddProperty(
        { .type = PropertyType::Float,
          .name = "Intensity",
          .offset = offsetof(AmbientLightComponent, intensity),
          .size = sizeof(float),
          .OnChangedCallback = onPropertyChanged });
    StaticClass()->AddProperty(
        { .type = PropertyType::Color,
          .name = "Color",
          .offset = offsetof(AmbientLightComponent, color),
          .size = sizeof(Color),
          .OnChangedCallback = onPropertyChanged });
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
