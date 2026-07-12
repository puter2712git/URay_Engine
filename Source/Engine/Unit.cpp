#include "Unit.h"

#include "Engine/Component/Component.h"

namespace URay
{

void Unit::Update(float deltaTime)
{
    for (auto comp : components)
        comp->Update(deltaTime);
}

Component* Unit::AddComponent(Component* comp)
{
    if (!comp)
        return nullptr;

    components.insert(comp);
    return comp;
}

} // namespace URay
