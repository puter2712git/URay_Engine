#pragma once

#include "Engine/Object/Object.h"

#include "Core/File/VirtualPath.h"

#include <functional>
#include <map>
#include <memory>
#include <vector>

namespace URay
{

class Unit;

namespace Render
{
class RenderScene;
}

enum class SceneType
{
    Game,
    Editor,
};

struct UpdateGroup
{
    int priority = 0;
    std::vector<std::function<void(float)>> functions;
};

class Scene : public Object
{
public:
    Scene(SceneType type, const VirtualPath& filePath);
    ~Scene();

public:
    void Update(float deltaTime);

    virtual YAML::Node Serialize() override;
    virtual void Deserialize(const YAML::Node& node) override;

    void AddUnit(Unit* unit);

    const VirtualPath& GetFilePath() const { return filePath; }
    void SetFilePath(const VirtualPath& filePath) { this->filePath = filePath; }

    Render::RenderScene* GetRenderScene() const { return renderScene.get(); }
    SceneType GetType() const { return type; }
    const std::vector<Unit*>& GetUnits() const { return units; }

private:
    VirtualPath filePath = {};

    std::unique_ptr<Render::RenderScene> renderScene = nullptr;

    SceneType type = SceneType::Game;
    std::vector<Unit*> units;

    std::map<int, UpdateGroup, std::greater<int>> updateGroups;
};

} // namespace URay
