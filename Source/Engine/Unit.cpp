#include "Unit.h"

#include "Engine/Component/Component.h"
#include "Engine/Component/TransformComponent.h"

namespace URay
{

URAY_REGISTER_CLASS(Unit)

Unit::~Unit()
{
    for (Component* comp : components)
    {
        if (comp)
        {
            delete comp;
            comp = nullptr;
        }
    }

    components.clear();
}

void Unit::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty({ PropertyType::String, "Name", offsetof(Unit, name), sizeof(std::string) });
}

void Unit::Update(float deltaTime)
{
    for (auto comp : components)
        comp->Update(deltaTime);
}

Component* Unit::AddComponent(Component* comp)
{
    if (!comp)
        return nullptr;

    if (TransformComponent* transformComp = dynamic_cast<TransformComponent*>(comp))
    {
        transform = transformComp;
    }

    comp->SetOwner(this);

    components.insert(comp);
    return comp;
}

} // namespace URay
