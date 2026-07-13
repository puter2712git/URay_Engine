#pragma once

#include <vector>

namespace URay
{

class Unit;

class Scene
{
public:
    Scene() = default;
    ~Scene();

public:
    void Update(float deltaTime);

    void AddUnit(Unit* unit);

private:
    std::vector<Unit*> units;
};

} // namespace URay
