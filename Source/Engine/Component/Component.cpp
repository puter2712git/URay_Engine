#include "Component.h"

#include "Engine/Object/Class/Class.h"

namespace URay
{

URAY_REGISTER_CLASS(Component)

void Component::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty({ PropertyType::Bool, "Enable", offsetof(Component, enabled), sizeof(bool) });
}

void Component::Update(float deltaTime)
{
}

void Component::GetProperties(std::vector<Property>& properties)
{
    properties.emplace_back(PropertyType::Bool, "Enable", offsetof(Component, enabled), sizeof(bool));
}

} // namespace URay
