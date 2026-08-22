#pragma once

#include "SceneType.h"

#include "Engine/Object/Object.h"

#include <vector>

namespace URay
{

class Unit;

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

private:
    SceneType type = SceneType::Game;

    std::vector<Unit*> units;
};

} // namespace URay
