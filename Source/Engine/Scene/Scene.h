#pragma once

#include "Engine/Object/Object.h"

#include "Core/File/VirtualPath.h"

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

    void SetRenderScene(Render::RenderScene* renderScene) { this->renderScene = renderScene; }

    Render::RenderScene* GetRenderScene() const { return renderScene; }
    SceneType GetType() const { return type; }
    const std::vector<Unit*>& GetUnits() const { return units; }

private:
    VirtualPath filePath = {};

    Render::RenderScene* renderScene = nullptr;

    SceneType type = SceneType::Game;
    std::vector<Unit*> units;
};

} // namespace URay
