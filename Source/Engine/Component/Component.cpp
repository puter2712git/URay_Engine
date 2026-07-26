#include "Component.h"

namespace URay
{

void Component::Update(float deltaTime)
{
}

void Component::GetProperties(std::vector<Property>& properties)
{
    properties.emplace_back(PropertyType::Bool, "Enable", offsetof(Component, enabled), sizeof(bool));
}

} // namespace URay
