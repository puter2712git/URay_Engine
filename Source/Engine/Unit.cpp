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

YAML::Node Unit::Serialize()
{
    YAML::Node node;

    for (auto comp : components)
        node[comp->GetClass()->GetName()] = comp->Serialize();

    return node;
}

void Unit::Deserialize(const YAML::Node& node)
{
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
