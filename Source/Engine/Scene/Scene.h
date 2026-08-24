#pragma once

#include "SceneType.h"

#include "Engine/Object/Object.h"

#include <memory>
#include <vector>

namespace URay
{

class Unit;
class Octree;

namespace Render
{
class RenderScene;
}

class Scene : public Object
{
public:
    Scene(SceneType type);
    ~Scene();

public:
    void Update(float deltaTime);

    virtual YAML::Node Serialize() override;
    virtual void Deserialize(const YAML::Node& node) override;

    void AddUnit(Unit* unit);

    void SetRenderScene(Render::RenderScene* renderScene) { this->renderScene = renderScene; }

    Render::RenderScene* GetRenderScene() const { return renderScene; }
    SceneType GetType() const { return type; }
    const std::vector<Unit*>& GetUnits() const { return units; }
    Octree* GetOctree() const { return octree.get(); }

private:
    Render::RenderScene* renderScene = nullptr;

    SceneType type = SceneType::Game;
    std::vector<Unit*> units;

    std::unique_ptr<Octree> octree = nullptr;
};

} // namespace URay
