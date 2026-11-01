#include "Scene.h"

#include <Core/Log/LogSystem.h>
#include <Core/Serialization/YAMLConverters.h>
#include <Core/Type/TypeAlias.h>
#include <Engine/Component/ComponentFactory.h>
#include <Engine/Component/Render/RenderComponent.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Scene/SceneSystem.h>
#include <Engine/Scene/Unit.h>
#include <Render/Rendering/Object/RenderObject.h>
#include <Render/Rendering/Scene/RenderScene.h>

#include <algorithm>
#include <iterator>
#include <unordered_map>

namespace URay
{

URAY_REGISTER_CLASS(Scene)

void Scene::RegisterClass() {}

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
    sceneNode["Version"] = 5;

    HashMap<const Unit*, uint32> siblingIndices;

    for (uint32 i = 0; i < rootUnits.size(); ++i)
    {
        siblingIndices.emplace(rootUnits[i], i);
    }

    for (const auto& unit : units)
    {
        const auto& children = unit->GetChildren();

        for (uint32 i = 0; i < children.size(); ++i)
        {
            siblingIndices.emplace(children[i], i);
        }
    }

    YAML::Node unitNodes(YAML::NodeType::Sequence);

    for (const auto& unit : units)
    {
        YAML::Node unitNode;
        unitNode["Id"] = unit->GetPersistentId();
        unitNode["SiblingIndex"] = siblingIndices.at(unit.get());

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

    HashMap<UUID, Unit*, UUIDHash> unitsById;
    HashMap<Unit*, uint32> siblingIndices;
    Vector<ParentLink> pendingParentLinks;

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
            URAY_LOG("Failed to deserialize unit: %s", unitNode["Name"].as<String>());
            continue;
        }

        auto newUnit = MakeUnique<Unit>();

        newUnit->SetPersistentId(id);
        unitsById.emplace(id, newUnit.get());

        if (const YAML::Node nameNode = unitNode["Name"])
            newUnit->SetName(nameNode.as<String>());

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

            const String typeName = typeNode.as<String>();

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

        const YAML::Node siblingIndexNode = unitNode["SiblingIndex"];
        if (siblingIndexNode && siblingIndexNode.IsScalar())
        {
            try
            {
                siblingIndices.emplace(newUnit.get(), siblingIndexNode.as<uint32>());
            }
            catch (const YAML::Exception&)
            {
                URAY_LOG("Failed to read Unit sibling index.");
            }
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

    const auto restoreSiblingOrder =
        [&siblingIndices](Vector<Unit*> siblings, auto moveUnit)
    {
        Vector<bool> usedIndices(siblings.size(), false);

        for (Unit* unit : siblings)
        {
            const auto it = siblingIndices.find(unit);
            if (it == siblingIndices.end() ||
                it->second >= siblings.size() ||
                usedIndices[it->second])
            {
                return;
            }

            usedIndices[it->second] = true;
        }

        std::sort(
            siblings.begin(),
            siblings.end(),
            [&siblingIndices](Unit* left, Unit* right)
            {
                return siblingIndices.at(left) < siblingIndices.at(right);
            });

        for (uint32 index = 0; index < siblings.size(); ++index)
        {
            moveUnit(siblings[index], index);
        }
    };

    restoreSiblingOrder(
        rootUnits,
        [this](Unit* unit, uint32 index)
        {
            MoveRootUnit(unit, index);
        });

    for (const auto& unit : units)
    {
        restoreSiblingOrder(
            unit->GetChildren(),
            [parent = unit.get()](Unit* child, uint32 index)
            {
                parent->MoveChild(child, index);
            });
    }
}

void Scene::AddUnit(UniquePtr<Unit> unit)
{
    Unit* unitPtr = unit.get();

    units.push_back(std::move(unit));
    unitPtr->SetOwner(this);
    rootUnits.push_back(unitPtr);

    sceneSystem.Emit("UnitAdded", this, unitPtr);

    const auto& components = unitPtr->GetComponents();
    for (const auto& component : components)
    {
        component->Initialize();
        sceneSystem.Emit("ComponentAdded", this, unitPtr, component.get());

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
                           [unit](const UniquePtr<Unit>& candidate)
                           { return candidate.get() == unit; });

    if (it == units.end())
        return;

    const Vector<Unit*> children = unit->GetChildren();

    for (Unit* child : children)
    {
        DestroyUnit(child);
    }

    it = std::find_if(units.begin(), units.end(),
                      [unit](const UniquePtr<Unit>& candidate)
                      { return candidate.get() == unit; });

    if (it == units.end())
        return;

    Unit* targetUnit = it->get();
    targetUnit->SetParent(nullptr);

    for (const auto& component : targetUnit->GetComponents())
    {
        component->Unload();
        sceneSystem.Emit("ComponentDestroyed", this, unit, component.get());
        component->Finalize();
    }

    std::erase(rootUnits, targetUnit);

    sceneSystem.Emit("UnitRemoved", this, unit);
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
        [&id](const UniquePtr<Unit>& unit)
        { return unit->GetPersistentId() == id; });

    return it != units.end() ? it->get() : nullptr;
}

Unit* Scene::CreateEmptyUnit(const String& name, Unit* parent)
{
    auto unit = MakeUnique<Unit>();
    unit->SetName(name);

    Unit* unitPtr = unit.get();
    AddUnit(std::move(unit));

    if (parent)
    {
        unitPtr->SetParent(parent);
    }

    return unitPtr;
}

bool Scene::MoveRootUnit(Unit* unit, uint32 index)
{
    if (!unit || index >= rootUnits.size())
        return false;

    const auto current = std::find(rootUnits.begin(), rootUnits.end(), unit);
    if (current == rootUnits.end())
        return false;

    const uint32 currentIndex = static_cast<uint32>(std::distance(rootUnits.begin(), current));

    if (currentIndex == index)
        return false;

    if (currentIndex < index)
    {
        std::rotate(
            rootUnits.begin() + currentIndex,
            rootUnits.begin() + currentIndex + 1,
            rootUnits.begin() + index + 1);
    }
    else
    {
        std::rotate(
            rootUnits.begin() + index,
            rootUnits.begin() + currentIndex,
            rootUnits.begin() + currentIndex + 1);
    }

    sceneSystem.Emit("SceneHierarchyChanged", this);
    return true;
}

void Scene::OnUnitParentChanged(Unit* unit, Unit* oldParent)
{
    Unit* newParent = unit->GetParent();

    if (!oldParent && newParent)
    {
        std::erase(rootUnits, unit);
    }
    else if (oldParent && !newParent)
    {
        rootUnits.push_back(unit);
    }
}

} // namespace URay
