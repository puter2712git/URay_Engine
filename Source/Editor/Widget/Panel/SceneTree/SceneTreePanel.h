#pragma once
#include <Core/Type/TypeAlias.h>

#include <Core/Type/UUID.h>
#include <Editor/Widget/Panel/Panel.h>
#include <Engine/Scene/Scene.h>

#include <vector>

namespace URay
{

class ScrollView;
class SceneTreeVirtualContent;
class SceneTreeNode;
class SceneSystem;
class Unit;

class SceneTreePanel final : public Panel
{
    URAY_TYPE(SceneTreePanel, Panel)

    friend class SceneTreeVirtualContent;

public:
    SceneTreePanel();
    ~SceneTreePanel() override;

public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;

protected:
    void OnUpdate(float deltaTime) override;

private:
    struct TreeRow
    {
        SceneId sceneId = InvalidSceneId;
        UUID unitId = {};
        String name;
        uint32 depth = 0;
    };

private:
    void UpdateTree();
    void AddSubtree(const Unit& unit, uint32 depth);

private:
    SceneSystem* sceneSystem = nullptr;

    ScrollView* scrollView = nullptr;
    SceneTreeVirtualContent* virtualContent = nullptr;

    std::vector<TreeRow> rows;

    bool isTreeDirty = false;
};

} // namespace URay
