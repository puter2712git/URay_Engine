#include "Unit.h"

#include <Engine/Component/Component.h>
#include <Engine/Component/Render/RenderComponent.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Scene/Scene.h>
#include <Engine/Scene/SceneSystem.h>

#include <Render/Rendering/Object/RenderObject.h>
#include <Render/Rendering/Scene/RenderScene.h>

#include <algorithm>

namespace URay
{

URAY_REGISTER_CLASS(Unit)

Unit::Unit() = default;

Unit::~Unit()
{
    for (const auto& component : components)
    {
        component->Finalize();
    }
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
    YAML::Node componentsNode(YAML::NodeType::Sequence);

    for (const auto& component : components)
    {
        YAML::Node componentNode(YAML::NodeType::Map);

        componentNode["Type"] = component->GetClass()->GetType().name;
        componentNode["Data"] = component->Serialize();

        componentsNode.push_back(componentNode);
    }

    return componentsNode;
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
        componentPtr->Initialize();

        SceneSystem& sceneSystem = scene->GetSceneSystem();
        sceneSystem.GetComponentAddRay().Emit(scene, this, componentPtr);

        if (scene->IsLoaded())
        {
            componentPtr->Load();
        }
    }

    return componentPtr;
}

} // namespace URay
