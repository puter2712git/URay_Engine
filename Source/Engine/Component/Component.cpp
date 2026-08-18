#include "Component.h"

#include "Engine/Object/Class/Class.h"

namespace URay
{

URAY_REGISTER_CLASS(Component)

void Component::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty({ .type = PropertyType::Bool,
                                 .name = "Enable",
                                 .offset = offsetof(Component, enabled),
                                 .size = sizeof(bool) });
}

void Component::Update(float deltaTime)
{
}

} // namespace URay
