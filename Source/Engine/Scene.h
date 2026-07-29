#pragma once

#include "Engine/Object/Object.h"

#include <vector>

namespace URay
{

class Unit;

class Scene : public Object
{
public:
    Scene() = default;
    ~Scene();

public:
    void Update(float deltaTime);

    virtual YAML::Node Serialize() override;
    virtual void Deserialize(const YAML::Node& node) override;

    void AddUnit(Unit* unit);
    std::vector<Unit*> GetUnits() const
    {
        return units;
    }

private:
    std::vector<Unit*> units;
};

} // namespace URay
