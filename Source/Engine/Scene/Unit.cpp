#include "Unit.h"

#include "Engine/Component/Component.h"
#include "Engine/Component/Render/RenderComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"

#include "Render/Rendering/Object/RenderObject.h"
#include "Render/Rendering/Scene/RenderScene.h"

#include <algorithm>

namespace URay
{

URAY_REGISTER_CLASS(Unit)

Unit::Unit() = default;

Unit::~Unit()
{
    components.clear();
}

void Unit::RegisterClass()
{
    StaticClass()->AddProperty({ .type = PropertyType::String,
                                 .name = "Name",
                                 .offset = offsetof(Unit, name),
                                 .size = sizeof(std::string) });
}

void Unit::Update(float deltaTime)
{
    for (const auto& comp : components)
    {
        comp->Update(deltaTime);
    }
}

YAML::Node Unit::Serialize() const
{
    YAML::Node node;

    for (const auto& component : components)
    {
        node[component->GetClass()->GetName()] = component->Serialize();
    }

    return node;
}

void Unit::Deserialize(const YAML::Node& node)
{
}

bool Unit::SetParent(Unit* unit)
{
    if (unit == this)
        return false;

    for (Unit* ancestor = unit; ancestor; ancestor = ancestor->parent)
    {
        if (ancestor == this)
            return false;
    }

    if (parent == unit)
        return true;

    if (parent)
    {
        auto& siblings = parent->children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
    }

    parent = unit;

    if (parent)
    {
        parent->children.push_back(this);
    }

    return true;
}

Component* Unit::AddComponent(std::unique_ptr<Component> component)
{
    Component* componentPtr = component.get();

    if (TransformComponent* transformComp = Cast<TransformComponent>(componentPtr))
    {
        transform = transformComp;
    }

    component->SetOwner(this);
    components.push_back(std::move(component));

    if (scene)
    {
        if (RenderComponent* renderComp = Cast<RenderComponent>(componentPtr))
        {
            std::unique_ptr<Render::RenderObject> robj(renderComp->CreateRenderObject());
            if (robj && scene->GetRenderScene())
            {
                scene->GetRenderScene()->Add(std::move(robj));
            }
        }
    }

    return componentPtr;
}

} // namespace URay
