#pragma once

#include "SceneType.h"

#include "Engine/Object/Object.h"

#include <memory>
#include <vector>

namespace URay
{

class Unit;
class Octree;

class Scene : public Object
{
public:
    Scene(SceneType type);
    ~Scene();

public:
    void Update(float deltaTime);

    virtual YAML::Node Serialize() override;
    virtual void Deserialize(const YAML::Node& node) override;

    SceneType GetType() const { return type; }

    void AddUnit(Unit* unit);
    const std::vector<Unit*>& GetUnits() const { return units; }

    Octree* GetOctree() const { return octree.get(); }

private:
    SceneType type = SceneType::Game;

    std::vector<Unit*> units;

    std::unique_ptr<Octree> octree = nullptr;
};

} // namespace URay
