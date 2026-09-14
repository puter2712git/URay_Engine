#include "Scene.h"

#include "Engine/Component/ComponentFactory.h"
#include "Engine/Component/Render/RenderComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

#include "Core/Log/Log.h"
#include "Core/Type/Types.h"

#include "Render/Scene/Object/RenderObject.h"
#include "Render/Scene/RenderScene.h"

namespace URay
{

Scene::Scene(SceneSystem& sceneSystem, SceneType type, const VirtualPath& filePath)
    : sceneSystem(sceneSystem), type(type), filePath(filePath)
{
    renderScene = std::make_unique<Render::RenderScene>();
}

Scene::~Scene()
{
    for (auto& unit : units)
    {
        sceneSystem.GetUnitRemoveRay().Emit(this, unit.get());
    }
    units.clear();
}

void Scene::Update(float deltaTime)
{
    updateGroups.clear();

    for (const auto& unit : units)
    {
        for (const auto& component : unit->GetComponents())
        {
            if (TransformComponent* transform = Cast<TransformComponent>(component.get()))
            {
                UpdateGroup& group = updateGroups[-100];
                group.functions.emplace_back([transform](float dt)
                                             { transform->Update(dt); });
            }
            else if (RenderComponent* renderComp = Cast<RenderComponent>(component.get()))
            {
                UpdateGroup& group = updateGroups[-101];
                group.functions.emplace_back([renderComp](float dt)
                                             { renderComp->Update(dt); });
            }
            else
            {
                Component* componentPtr = component.get();
                UpdateGroup& group = updateGroups[0];
                group.functions.emplace_back([componentPtr](float dt)
                                             { componentPtr->Update(dt); });
            }
        }
    }

    for (auto& [prio, group] : updateGroups)
    {
        for (auto& func : group.functions)
        {
            func(deltaTime);
        }
    }
}

YAML::Node Scene::Serialize() const
{
    YAML::Node sceneNode;
    sceneNode["Version"] = 1;

    YAML::Node unitNodes(YAML::NodeType::Sequence);

    std::unordered_map<const Unit*, uint64> ids;
    uint64 nextId = 1;

    for (const auto& unit : units)
    {
        if (unit)
        {
            ids.emplace(unit.get(), nextId++);
        }
    }

    for (const auto& unit : units)
    {
        YAML::Node unitNode;
        unitNode["Id"] = ids.at(unit.get());

        const Unit* parent = unit->GetParent();
        if (parent)
        {
            const auto parentIter = ids.find(parent);

            if (parentIter != ids.end())
            {
                unitNode["ParentId"] = parentIter->second;
            }
            else
            {
                unitNode["ParentId"] = YAML::Node(YAML::NodeType::Null);
            }
        }
        else
        {
            unitNode["ParentId"] = YAML::Node(YAML::NodeType::Null);
        }

        unitNode["Name"] = unit->GetName();
        unitNode["Components"] = unit->Serialize();

        unitNodes.push_back(unitNode);
    }

    sceneNode["Units"] = unitNodes;
    return sceneNode;
}

void Scene::Deserialize(const YAML::Node& node)
{
    const YAML::Node unitNodes = node["Units"];

    if (!unitNodes || !unitNodes.IsSequence())
    {
        Logger::Log("Failed to deserialize scene: Units must be a sequence.");
        return;
    }

    struct ParentLink
    {
        Unit* child = nullptr;
        uint64 parentId = 0;
    };

    std::unordered_map<uint64, Unit*> unitsById;
    std::vector<ParentLink> pendingParentLinks;

    for (const YAML::Node& unitNode : unitNodes)
    {
        if (!unitNode.IsMap() || !unitNode["Id"])
        {
            Logger::Log("Failed to deserialize scene: Unit id is missing.");
            continue;
        }

        const uint64 id = unitNode["Id"].as<uint64>();

        if (unitsById.contains(id))
        {
            Logger::Log("Failed to deserialize scene: Duplicate Unit Id.");
            continue;
        }

        std::unique_ptr<Unit> newUnit = std::make_unique<Unit>();

        if (const YAML::Node nameNode = unitNode["Name"])
            newUnit->SetName(nameNode.as<std::string>());

        const YAML::Node componentsNode = unitNode["Components"];
        if (componentsNode && componentsNode.IsMap())
        {
            for (const auto& compNode : componentsNode)
            {
                std::unique_ptr<Component> component = ComponentFactory::Create(compNode.first.as<std::string>());
                if (!component)
                    continue;

                component->Deserialize(compNode.second);
                newUnit->AddComponent(std::move(component));
            }
        }

        const YAML::Node parentIdNode = unitNode["ParentId"];
        if (parentIdNode && !parentIdNode.IsNull())
        {
            pendingParentLinks.push_back(ParentLink{
                .child = newUnit.get(),
                .parentId = parentIdNode.as<uint64>() });
        }

        unitsById.emplace(id, newUnit.get());
        AddUnit(std::move(newUnit));
    }

    for (const ParentLink& link : pendingParentLinks)
    {
        const auto parentIter = unitsById.find(link.parentId);

        if (parentIter == unitsById.end())
        {
            Logger::Log("Failed to deserialize scene: Parent Unit Id was not found.");
            continue;
        }

        if (!link.child->SetParent(parentIter->second))
        {
            Logger::Log("Failed to deserialize scene: Invalid parent relationship.");
        }
    }
}

void Scene::AddUnit(std::unique_ptr<Unit> unit)
{
    Unit* unitPtr = unit.get();

    units.push_back(std::move(unit));
    unitPtr->SetOwner(this);

    sceneSystem.GetUnitAddRay().Emit(this, unitPtr);

    const auto& components = unitPtr->GetComponents();
    for (const auto& component : components)
    {
        if (RenderComponent* renderComp = Cast<RenderComponent>(component.get()))
        {
            std::unique_ptr<Render::RenderObject> robject(renderComp->CreateRenderObject());
            renderScene->Add(std::move(robject));
        }
    }
}

void Scene::DestroyUnit(Unit* unit)
{
    if (!unit)
        return;

    sceneSystem.GetUnitRemoveRay().Emit(this, unit);

    auto it = std::find_if(units.begin(), units.end(),
                           [unit](const std::unique_ptr<Unit>& candidate)
                           { return candidate.get() == unit; });
    if (it != units.end())
    {
        units.erase(it);
    }
}

} // namespace URay
