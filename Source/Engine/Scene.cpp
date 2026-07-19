#include "Scene.h"

#include "Engine/Unit.h"

namespace URay
{

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

void Scene::AddUnit(Unit* unit)
{
    if (unit)
        units.push_back(unit);
}

} // namespace URay
