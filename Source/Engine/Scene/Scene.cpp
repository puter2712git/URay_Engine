#include "Scene.h"

#include <Core/Log/LogSystem.h>
#include <Core/Serialization/YAMLConverters.h>
#include <Core/Type/Types.h>
#include <Engine/Component/ComponentFactory.h>
#include <Engine/Component/Render/RenderComponent.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Scene/SceneSystem.h>
#include <Engine/Scene/Unit.h>
#include <Render/Rendering/Object/RenderObject.h>
#include <Render/Rendering/Scene/RenderScene.h>

namespace URay
{

Scene::Scene(SceneSystem& sceneSystem, SceneId id, SceneType type, const VirtualPath& filePath)
    : sceneSystem(sceneSystem), id(id), type(type), filePath(filePath) {}

void Scene::Load()
{
    if (loaded)
        return;

    loaded = true;

    for (const auto& unit : units)
    {
        for (const auto& comp : unit->GetComponents())
        {
            comp->Load();
        }
    }
}

void Scene::Unload()
{
    if (!loaded)
        return;

    for (const auto& unit : units)
    {
        for (const auto& comp : unit->GetComponents())
        {
            comp->Unload();
        }
    }

    loaded = false;
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
    sceneNode["Version"] = 4;

    YAML::Node unitNodes(YAML::NodeType::Sequence);

    for (const auto& unit : units)
    {
        YAML::Node unitNode;
        unitNode["Id"] = unit->GetPersistentId();

        if (const Unit* parent = unit->GetParent())
            unitNode["ParentId"] = parent->GetPersistentId();
        else
            unitNode["ParentId"] = YAML::Node(YAML::NodeType::Null);

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
        URAY_LOG("Failed to deserialize scene: Units must be a sequence.");
        return;
    }

    struct ParentLink
    {
        Unit* child = nullptr;
        UUID parentId = {};
    };

    std::unordered_map<UUID, Unit*, UUIDHash> unitsById;
    std::vector<ParentLink> pendingParentLinks;

    for (const YAML::Node& unitNode : unitNodes)
    {
        if (!unitNode.IsMap() || !unitNode["Id"])
        {
            URAY_LOG("Failed to deserialize scene: Unit id is missing.");
            continue;
        }

        const UUID id = unitNode["Id"].as<UUID>();

        if (!id.IsValid() || unitsById.contains(id))
        {
            URAY_LOG("Failed to deserialize unit: %s", unitNode["Name"].as<std::string>());
            continue;
        }

        auto newUnit = std::make_unique<Unit>();

        newUnit->SetPersistentId(id);
        unitsById.emplace(id, newUnit.get());

        if (const YAML::Node nameNode = unitNode["Name"])
            newUnit->SetName(nameNode.as<std::string>());

        const YAML::Node componentsNode = unitNode["Components"];

        for (const YAML::Node componentNode : componentsNode)
        {
            if (!componentNode.IsMap())
            {
                URAY_LOG(
                    "Failed to deserialize scene: "
                    "Component entry must be a map.");
                continue;
            }

            const YAML::Node typeNode = componentNode["Type"];
            const YAML::Node dataNode = componentNode["Data"];

            if (!typeNode || !typeNode.IsScalar() ||
                !dataNode || !dataNode.IsMap())
            {
                URAY_LOG(
                    "Failed to deserialize scene: "
                    "Component entry requires Type and Data.");
                continue;
            }

            const std::string typeName = typeNode.as<std::string>();

            auto component = ComponentFactory::Create(typeName);
            if (!component)
            {
                URAY_LOG(
                    "Failed to deserialize component: %s",
                    typeName.c_str());
                continue;
            }

            component->Deserialize(dataNode);
            newUnit->AddComponent(std::move(component));
        }

        const YAML::Node parentIdNode = unitNode["ParentId"];
        if (parentIdNode && !parentIdNode.IsNull())
        {
            pendingParentLinks.push_back(ParentLink{
                .child = newUnit.get(),
                .parentId = parentIdNode.as<UUID>() });
        }

        AddUnit(std::move(newUnit));
    }

    for (const ParentLink& link : pendingParentLinks)
    {
        const auto parentIter = unitsById.find(link.parentId);

        if (parentIter == unitsById.end())
        {
            URAY_LOG("Failed to deserialize scene: Parent Unit Id was not found.");
            continue;
        }

        if (!link.child->SetParent(parentIter->second))
        {
            URAY_LOG("Failed to deserialize scene: Invalid parent relationship.");
        }
    }
}

void Scene::AddUnit(std::unique_ptr<Unit> unit)
{
    Unit* unitPtr = unit.get();

    units.push_back(std::move(unit));
    unitPtr->SetOwner(this);

    sceneSystem.onUnitAdded.Emit(this, unitPtr);

    const auto& components = unitPtr->GetComponents();
    for (const auto& component : components)
    {
        component->Initialize();
        sceneSystem.onComponentAdded.Emit(this, unitPtr, component.get());

        if (IsLoaded())
        {
            component->Load();
        }
    }
}

void Scene::DestroyUnit(Unit* unit)
{
    if (!unit)
        return;

    auto it = std::find_if(units.begin(), units.end(),
                           [unit](const std::unique_ptr<Unit>& candidate)
                           { return candidate.get() == unit; });

    if (it == units.end())
        return;

    const std::vector<Unit*> children = unit->GetChildren();

    for (Unit* child : children)
    {
        DestroyUnit(child);
    }

    it = std::find_if(units.begin(), units.end(),
                      [unit](const std::unique_ptr<Unit>& candidate)
                      { return candidate.get() == unit; });

    if (it == units.end())
        return;

    Unit* targetUnit = it->get();
    targetUnit->SetParent(nullptr);

    for (const auto& component : targetUnit->GetComponents())
    {
        component->Unload();
        sceneSystem.onComponentDestroyed.Emit(this, unit, component.get());
        component->Finalize();
    }

    sceneSystem.onUnitRemoved.Emit(this, unit);
    units.erase(it);
}

void Scene::DestroyAllUnits()
{
    while (!units.empty())
    {
        DestroyUnit(units.back().get());
    }
}

Unit* Scene::FindUnit(const UUID& id) const
{
    const auto it = std::find_if(
        units.begin(), units.end(),
        [&id](const std::unique_ptr<Unit>& unit)
        { return unit->GetPersistentId() == id; });

    return it != units.end() ? it->get() : nullptr;
}

Unit* Scene::CreateEmptyUnit(const std::string& name, Unit* parent)
{
    auto unit = std::make_unique<Unit>();
    unit->SetName(name);

    Unit* unitPtr = unit.get();
    AddUnit(std::move(unit));

    if (parent)
    {
        unitPtr->SetParent(parent);
    }

    return unitPtr;
}

} // namespace URay
