#pragma once

#include <Engine/Object/ObjectId.h>

#include <optional>

namespace URay
{

class ObjectSystem;
class UISystem;

class DockSystem
{
public:
    DockSystem(UISystem& uiSystem, ObjectSystem& objectSystem);

public:
    void RequestMove(ObjectId sourceGroupId, ObjectId targetGroupId, ObjectId panelId);

    void ProcessPendingMove();

private:
    struct MoveRequest
    {
        ObjectId sourceGroupId;
        ObjectId targetGroupId;
        ObjectId panelId;
    };

    UISystem& uiSystem;
    ObjectSystem& objectSystem;

    std::optional<MoveRequest> pendingMove;
};

} // namespace URay
