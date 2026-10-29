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

public:
    DockSystem(UISystem& uiSystem, ObjectSystem& objectSystem);

public:
    void RequestMove(ObjectId sourceGroupId, ObjectId targetGroupId, ObjectId panelId, DockDropZone zone);
    void RequestDrop(ObjectId sourceGroupId, ObjectId panelId, const Vector2& position);

    void UpdateDrag(ObjectId sourceGroupId, ObjectId panelId, const Vector2& position);

    void ProcessPendingMove();
    void ProcessPendingCleanup();

private:
    bool SplitGroup(DockTabGroup& target, DockDropZone zone, std::unique_ptr<Widget>& newGroup);
    bool CanSplitGroup(const DockTabGroup& target, DockDropZone zone) const;

    std::optional<DockDropTarget> FindDockDropTarget(ObjectId sourceGroupId, const Vector2& position) const;
    void ClearDropHighlight();

    bool TryCleanupEmptyGroup(ObjectId groupId);

private:
    UISystem& uiSystem;
    ObjectSystem& objectSystem;

    std::optional<MoveRequest> pendingMove;
    std::vector<ObjectId> pendingCleanups;

    std::optional<DockDropTarget> highlightedTarget;
};

} // namespace URay
