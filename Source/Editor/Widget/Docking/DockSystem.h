#pragma once

#include <Core/Math/Vector2.h>
#include <Editor/Widget/Docking/DockDropTarget.h>
#include <Engine/Object/ObjectId.h>

#include <memory>
#include <optional>
#include <vector>

namespace URay
{

class ObjectSystem;
class UISystem;
class DockTabGroup;
class Widget;

class DockSystem
{
    struct MoveRequest
    {
        ObjectId sourceGroupId;
        ObjectId targetGroupId;
        ObjectId panelId;
        DockDropZone zone = DockDropZone::Center;
    };

    struct DragState
    {
        ObjectId sourceGroupId = {};
        ObjectId panelId = {};
    };

public:
    DockSystem(UISystem& uiSystem, ObjectSystem& objectSystem);

public:
    void RequestMove(ObjectId sourceGroupId, ObjectId targetGroupId, ObjectId panelId, DockDropZone zone);
    void RequestDrop(ObjectId sourceGroupId, ObjectId panelId, const Vector2& position);

    void UpdateDrag(ObjectId sourceGroupId, ObjectId panelId, const Vector2& position);
    void CancelDrag();

    void ProcessPendingMove();
    void ProcessPendingCleanup();

private:
    bool SplitGroup(DockTabGroup& target, DockDropZone zone, std::unique_ptr<Widget>& newGroup);
    bool CanSplitGroup(const DockTabGroup& target, DockDropZone zone) const;

    DockDropResult FindDockDropTarget(ObjectId sourceGroupId, const Vector2& position) const;
    void ClearDropHighlight();

    bool TryCleanupEmptyGroup(ObjectId groupId);

    DockTabGroup* FindIndicatorGroup(ObjectId sourceGroupId, const Vector2& position) const;
    void ClearIndicatorGroup();

private:
    UISystem& uiSystem;
    ObjectSystem& objectSystem;

    std::optional<DragState> dragState;

    std::optional<MoveRequest> pendingMove;
    std::vector<ObjectId> pendingCleanups;

    std::optional<ObjectId> indicatorGroupId;
    std::optional<DockDropTarget> highlightedTarget;
};

} // namespace URay
