#include "Scene.h"

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
}

void Scene::AddUnit(Unit* unit)
{
    if (unit)
        units.push_back(unit);
}

} // namespace URay
