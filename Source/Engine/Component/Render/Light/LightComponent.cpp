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
    StaticClass()->AddProperty(
        { .type = PropertyType::Float,
          .name = "Intensity",
          .offset = offsetof(LightComponent, intensity),
          .size = sizeof(float) });
    StaticClass()->AddProperty(
        { .type = PropertyType::Color,
          .name = "Color",
          .offset = offsetof(LightComponent, color),
          .size = sizeof(Color) });
}

} // namespace URay
