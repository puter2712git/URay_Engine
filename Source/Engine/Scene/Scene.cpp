#include "Scene.h"

#include "Engine/Component/ComponentFactory.h"
#include "Engine/Component/MeshComponent.h"
#include "Engine/Spatial/Octree.h"
#include "Engine/Unit.h"

#include "Render/Scene/Object/RenderObject.h"
#include "Render/Scene/RenderScene.h"

#include "Core/Math/AABB.h"

namespace URay
{

Scene::Scene(SceneType type)
    : type(type)
{
    octree = std::make_unique<Octree>(AABB{
        .min = Vector3(-100.0f, -100.0f, -100.0f),
        .max = Vector3(100.0f, 100.0f, 100.0f),
    });
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

    const auto& components = unit->GetComponents();
    for (const auto& comp : components)
    {
        if (IRenderable* rcomp = dynamic_cast<IRenderable*>(comp))
        {
            std::unique_ptr<Render::RenderObject> robject(rcomp->CreateRenderObject());
            renderScene->Add(std::move(robject));
        }
    }
}

} // namespace URay
