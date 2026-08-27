#include "Scene.h"

#include "Engine/Component/ComponentFactory.h"
#include "Engine/Component/Render/RenderComponent.h"
#include "Engine/Scene/Unit.h"

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
    for (Unit* unit : units)
    {
        if (unit)
            unit->Update(deltaTime);
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
        if (RenderComponent* renderComp = dynamic_cast<RenderComponent*>(comp))
        {
            std::unique_ptr<Render::RenderObject> robject(renderComp->CreateRenderObject());
            renderScene->Add(std::move(robject));
        }
    }
}

} // namespace URay
