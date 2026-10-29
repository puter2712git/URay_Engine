#pragma once

#include <Core/Math/Vector2.h>
#include <Engine/Object/ObjectId.h>

#include <optional>

namespace URay
{

class ObjectSystem;
class UISystem;
class DockTabGroup;

class DockSystem
{
    struct MoveRequest
    {
        ObjectId sourceGroupId;
        ObjectId targetGroupId;
        ObjectId panelId;
    };

public:
    DockSystem(UISystem& uiSystem, ObjectSystem& objectSystem);

public:
    void RequestMove(ObjectId sourceGroupId, ObjectId targetGroupId, ObjectId panelId);
    void RequestDrop(ObjectId sourceGroupId, ObjectId panelId, const Vector2& position);

    void UpdateDrag(ObjectId sourceGroupId, ObjectId panelId, const Vector2& position);

    void ProcessPendingMove();

private:
    DockTabGroup* FindDropTarget(ObjectId sourceGroupId, const Vector2& position) const;
    void ClearDropHighlight();

private:
    UISystem& uiSystem;
    ObjectSystem& objectSystem;

    std::optional<MoveRequest> pendingMove;

    std::optional<ObjectId> highlightedGroupId;
};

} // namespace URay
