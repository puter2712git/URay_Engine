#include "Scene.h"

#include "Engine/Component/ComponentFactory.h"
#include "Engine/Unit.h"

namespace URay
{

Scene::Scene(SceneType type)
    : type(type)
{
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
    for (Unit* unit : units)
    {
        if (unit)
            unit->Update(deltaTime);
    }
}

YAML::Node Scene::Serialize()
{
    YAML::Node node;

    for (Unit* unit : units)
        node[unit->GetName()] = unit->Serialize();

    return node;
}

void Scene::Deserialize(const YAML::Node& node)
{
    for (const auto& unitNode : node)
    {
        Unit* newUnit = new Unit();
        newUnit->SetName(unitNode.first.as<std::string>());

        for (const auto& compNode : unitNode.second)
        {
            Component* comp = ComponentFactory::Create(compNode.first.as<std::string>());
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
}

} // namespace URay
