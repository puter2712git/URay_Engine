#include "PointLightComponent.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

#include "Render/Scene/Object/Light/PointLightObject.h"

namespace URay
{

URAY_REGISTER_CLASS(PointLightComponent)
URAY_REGISTER_COMPONENT(PointLightComponent)

void PointLightComponent::RegisterClass()
{
    const auto onPropertyChanged = [](Object* owner, const Property& property)
    {
        PointLightComponent* comp = static_cast<PointLightComponent*>(owner);
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
          .name = "Radius",
          .offset = offsetof(PointLightComponent, radius),
          .size = sizeof(float),
          .OnChangedCallback = onPropertyChanged });
}

PointLightComponent::PointLightComponent() = default;

PointLightComponent::~PointLightComponent() = default;

Render::RenderObject* PointLightComponent::CreateRenderObject()
{
    Unit* unit = GetOwner();
    if (!unit)
        return nullptr;

    TransformComponent* transform = unit->GetTransform();

    Render::PointLightObjectState state = {};
    state.position = transform ? transform->GetPosition() : Vector3::Zero;
    state.radius = radius;
    state.intensity = intensity;
    state.color = color;

    renderObject = new Render::PointLightObject(state);
    return renderObject;
}

void PointLightComponent::UpdateRenderObject()
{
    Unit* unit = GetOwner();
    if (!unit)
        return;

    TransformComponent* transform = unit->GetTransform();

    Render::PointLightObjectState state = {};
    state.position = transform ? transform->GetPosition() : Vector3::Zero;
    state.radius = radius;
    state.intensity = intensity;
    state.color = color;

    Render::PointLightObject* object =
        static_cast<Render::PointLightObject*>(renderObject);
    object->Update(state);
}

} // namespace URay
