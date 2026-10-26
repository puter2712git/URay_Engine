#include "Unit.h"
#include <Core/Type/TypeAlias.h>

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
                                 .size = sizeof(String) });
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

bool Unit::SetParent(Unit* unit, ReparentMode mode)
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

    std::vector<TransformComponent*> transformRoots;
    std::vector<Matrix> localMatrices;

    if (mode == ReparentMode::KeepWorld)
    {
        CollectTransformRoots(transformRoots, false);

        TransformComponent* newRelativeTransform = FindClosestTransform(unit);

        const Matrix newRelativeWorld = newRelativeTransform ? newRelativeTransform->GetWorldMatrix() : Matrix::Identity;
        const Matrix inverseNewRelativeWorld = newRelativeWorld.Inverse();

        localMatrices.reserve(transformRoots.size());

        for (TransformComponent* transform : transformRoots)
        {
            localMatrices.push_back(transform->GetWorldMatrix() * inverseNewRelativeWorld);
        }
    }

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

    for (size_t i = 0; i < transformRoots.size(); ++i)
    {
        const Matrix& local = localMatrices[i];

        transformRoots[i]->SetLocalPosition(local.GetPosition());
        transformRoots[i]->SetLocalRotation(local.GetRotation());
        transformRoots[i]->SetLocalScale(local.GetScale());
    }

    MarkTransformHierarchyDirty();
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
        sceneSystem.onComponentAdded.Emit(scene, this, componentPtr);

        if (scene->IsLoaded())
        {
            componentPtr->Load();
        }
    }

    return componentPtr;
}

TransformComponent* Unit::FindClosestTransform(Unit* unit)
{
    for (; unit; unit = unit->parent)
    {
        if (unit->transform)
            return unit->transform;
    }

    return nullptr;
}

void Unit::CollectTransformRoots(
    std::vector<TransformComponent*>& results,
    bool hasTransformAncestor) const
{
    const bool hasTransform = transform != nullptr;

    if (hasTransform && !hasTransformAncestor)
        results.push_back(transform);

    for (Unit* child : children)
    {
        child->CollectTransformRoots(
            results,
            hasTransformAncestor || hasTransform);
    }
}

void Unit::MarkTransformHierarchyDirty()
{
    if (transform)
        transform->SetDirty(true);

    for (Unit* child : children)
        child->MarkTransformHierarchyDirty();
}

} // namespace URay
