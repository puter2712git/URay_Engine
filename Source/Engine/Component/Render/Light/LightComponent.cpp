#include "LightComponent.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

namespace URay
{

URAY_REGISTER_CLASS(LightComponent)

void LightComponent::RegisterClass()
{
    Super::RegisterClass();

    const auto onPropertyChanged = [](Object* owner, const Property& property)
    {
        LightComponent* comp = static_cast<LightComponent*>(owner);
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
          .offset = offsetof(LightComponent, intensity),
          .size = sizeof(float),
          .OnChangedCallback = onPropertyChanged });
    StaticClass()->AddProperty(
        { .type = PropertyType::Color,
          .name = "Color",
          .offset = offsetof(LightComponent, color),
          .size = sizeof(Color),
          .OnChangedCallback = onPropertyChanged });
}

} // namespace URay
