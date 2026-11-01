#pragma once

#include <Core/Math/Vector2.h>
#include <Core/Type/TypeAlias.h>
#include <Core/Type/UUID.h>
#include <Editor/Widget/Panel/Panel.h>
#include <Engine/Scene/Scene.h>

#include <optional>
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
    URAY_CLASS(SceneTreePanel, Panel)

    friend class SceneTreeVirtualContent;

    struct TreeRow
    {
        SceneId sceneId = InvalidSceneId;
        UUID unitId = {};
        String name;
        uint32 depth = 0;
    };

    struct DragState
    {
        SceneId sceneId = InvalidSceneId;
        UUID unitId = {};
        Vector2 pressPosition = Vector2::Zero;
        Vector2 pointerPosition = Vector2::Zero;
        bool isDragging = false;
    };

    enum class DropTargetType
    {
        None,
        Root,
        Unit,
        Insert
    };

    struct DropTarget
    {
        DropTargetType type = DropTargetType::None;
        SceneId sceneId = InvalidSceneId;
        UUID unitId = {};

        // Index 0: Insert before the first row
        // Index 1: Insert between the first and second rows
        // Index rows.size(): Insert after the last row
        int32 boundaryIndex = -1;
        UUID parentId = {};
        uint32 siblingIndex = 0;
    };

public:
    SceneTreePanel();
    ~SceneTreePanel() override;

public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerMove(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;
    void OnPointerCancel() override;

    void BeginDrag(SceneId sceneId, const UUID& unitId, const Vector2& position);

    bool IsUnitDropTarget(SceneId sceneId, const UUID& unitId) const;

protected:
    void OnUpdate(float deltaTime) override;
    void OnPaintForeground(Render::UIDrawContext& context) override;

private:
    void UpdateTree();
    void AddSubtree(const Unit& unit, uint32 depth);

    void OnSceneHierarchyChanged(Scene* scene);
    void OnUnitAdded(Scene* scene, Unit* unit);
    void OnUnitRemoved(Scene* scene, Unit* unit);

    DropTarget FindDropTarget(const Vector2& position) const;
    DropTarget FindInsertTarget(int32 boundaryIndex, float pointerX) const;
    bool IsValidInsertTarget(const DropTarget& target) const;

    void ApplyDrop(const DropTarget& target);

private:
    SceneSystem* sceneSystem = nullptr;

    ScrollView* scrollView = nullptr;
    SceneTreeVirtualContent* virtualContent = nullptr;

    std::vector<TreeRow> rows;

    bool isTreeDirty = false;

    std::optional<DragState> dragState;
};

} // namespace URay
