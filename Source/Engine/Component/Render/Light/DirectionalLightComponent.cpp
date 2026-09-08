#include "DirectionalLightComponent.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

#include "Render/Scene/Object/Light/DirectionalLightObject.h"

namespace URay
{

URAY_REGISTER_CLASS(DirectionalLightComponent)
URAY_REGISTER_COMPONENT(DirectionalLightComponent)

void DirectionalLightComponent::RegisterClass()
{
    Super::RegisterClass();

    const auto onPropertyChanged = [](Object* owner, const Property& property)
    {
        DirectionalLightComponent* comp = static_cast<DirectionalLightComponent*>(owner);
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
          .offset = offsetof(DirectionalLightComponent, intensity),
          .size = sizeof(float),
          .OnChangedCallback = onPropertyChanged });
    StaticClass()->AddProperty(
        { .type = PropertyType::Color,
          .name = "Color",
          .offset = offsetof(DirectionalLightComponent, color),
          .size = sizeof(Color),
          .OnChangedCallback = onPropertyChanged });
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
