#pragma once
#include <Core/Type/TypeAlias.h>

#include <Engine/Object/Object.h>

#include <Core/File/VirtualPath.h>

#include <functional>
#include <map>
#include <memory>
#include <vector>

namespace URay::Render
{
class RenderScene;
} // namespace URay::Render

namespace URay
{

class Unit;
class SceneSystem;
struct UUID;

enum class SceneType
{
    Game,
    Play,
    Preview
};

using SceneId = uint64;
constexpr SceneId InvalidSceneId = 0;

struct UpdateGroup
{
    int priority = 0;
    std::vector<std::function<void(float)>> functions;
};

class Scene : public Object
{
    URAY_OBJECT(Scene, Object)

    friend class Unit;

public:
    Scene(SceneSystem& sceneSystem, SceneId id, SceneType type, const VirtualPath& filePath);

public:
    void Load();
    void Unload();

    void Update(float deltaTime);

    virtual YAML::Node Serialize() const override;
    virtual void Deserialize(const YAML::Node& node) override;

    void AddUnit(UniquePtr<Unit> unit);
    void DestroyUnit(Unit* unit);
    void DestroyAllUnits();

    Unit* FindUnit(const UUID& id) const;

    Unit* CreateEmptyUnit(const String& name, Unit* parent);

    SceneSystem& GetSceneSystem() { return sceneSystem; }

    SceneId GetId() const { return id; }

    const VirtualPath& GetFilePath() const { return filePath; }
    void SetFilePath(const VirtualPath& filePath) { this->filePath = filePath; }

    bool IsLoaded() const { return loaded; }

    SceneType GetSceneType() const { return type; }

    const std::vector<UniquePtr<Unit>>& GetUnits() const { return units; }
    const std::vector<Unit*>& GetRootUnits() const { return rootUnits; }
    bool MoveRootUnit(Unit* unit, uint32 index);

private:
    void OnUnitParentChanged(Unit* unit, Unit* oldParent);

private:
    SceneSystem& sceneSystem;
    SceneId id = InvalidSceneId;
    VirtualPath filePath;

    bool loaded = false;

    SceneType type = SceneType::Game;
    std::vector<UniquePtr<Unit>> units;
    std::vector<Unit*> rootUnits;

    std::map<int, UpdateGroup, std::greater<int>> updateGroups;
};

} // namespace URay
