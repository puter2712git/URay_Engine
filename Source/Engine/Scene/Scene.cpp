#include "Scene.h"

#include "Engine/Component/ComponentFactory.h"
#include "Engine/Component/Render/RenderComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Scene/Unit.h"

#include "Core/Log/Log.h"

#include "Render/Scene/Object/RenderObject.h"
#include "Render/Scene/RenderScene.h"

namespace URay
{

Scene::Scene(SceneType type, const VirtualPath& filePath)
    : type(type), filePath(filePath)
{
    renderScene = std::make_unique<Render::RenderScene>();
}

Scene::~Scene()
{
    for (Unit* unit : units)
    {
        if (unit)
        {
            delete unit;
            unit = nullptr;
        }
    }

    units.clear();
}

void Scene::Update(float deltaTime)
{
    updateGroups.clear();

    for (Unit* unit : units)
    {
        auto& comps = unit->GetComponents();

        for (Component* comp : comps)
        {
            if (TransformComponent* transform = Cast<TransformComponent>(comp))
            {
                UpdateGroup& group = updateGroups[-100];
                group.functions.emplace_back([transform](float dt)
                                             { transform->Update(dt); });
            }
            else if (RenderComponent* renderComp = Cast<RenderComponent>(comp))
            {
                UpdateGroup& group = updateGroups[-101];
                group.functions.emplace_back([renderComp](float dt)
                                             { renderComp->Update(dt); });
            }
            else
            {
                UpdateGroup& group = updateGroups[0];
                group.functions.emplace_back([comp](float dt)
                                             { comp->Update(dt); });
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

YAML::Node Scene::Serialize()
{
    YAML::Node unitNodes(YAML::NodeType::Sequence);

    for (Unit* unit : units)
    {
        YAML::Node unitNode;
        unitNode["Name"] = unit->GetName();
        unitNode["Components"] = unit->Serialize();
        unitNodes.push_back(unitNode);
    }

    return unitNodes;
}

void Scene::Deserialize(const YAML::Node& node)
{
    for (const YAML::Node& unitNode : node)
    {
        Unit* newUnit = new Unit();
        newUnit->SetName(unitNode["Name"].as<std::string>());

        for (const auto& compNode : unitNode["Components"])
        {
            Component* comp = ComponentFactory::Create(compNode.first.as<std::string>());
            if (!comp)
                continue;

            comp->Deserialize(compNode.second);
            newUnit->AddComponent(comp);
        }

        AddUnit(newUnit);
    }
}

void Scene::AddUnit(Unit* unit)
{
    if (!unit)
        return;

    unit->SetOwner(this);
    units.push_back(unit);

    const auto& components = unit->GetComponents();
    for (const auto& comp : components)
    {
        if (RenderComponent* renderComp = Cast<RenderComponent>(comp))
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

    auto it = std::find(units.begin(), units.end(), unit);
    if (it != units.end())
    {
        delete unit;
        units.erase(it);
    }
}

} // namespace URay
