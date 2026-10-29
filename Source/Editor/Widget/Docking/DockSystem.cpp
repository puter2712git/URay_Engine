#include "DockSystem.h"

#include <Editor/Widget/Docking/DockTabGroup.h>
#include <Engine/Object/ObjectSystem.h>
#include <Engine/UI/UISystem.h>

#include <utility>

namespace URay
{

namespace
{

bool IsInRoot(const Widget& widget, const Widget& root)
{
    for (const Widget* current = &widget; current; current = current->GetParent())
    {
        if (current == &root)
            return true;
    }
    return false;
}

} // namespace

DockSystem::DockSystem(UISystem& uiSystem, ObjectSystem& objectSystem)
    : uiSystem(uiSystem), objectSystem(objectSystem)
{
}

void DockSystem::RequestMove(ObjectId sourceGroupId, ObjectId targetGroupId, ObjectId panelId)
{
    pendingMove = MoveRequest{
        .sourceGroupId = sourceGroupId,
        .targetGroupId = targetGroupId,
        .panelId = panelId
    };
}

void DockSystem::ProcessPendingMove()
{
    if (!pendingMove.has_value())
        return;

    const MoveRequest& request = *pendingMove;

    DockTabGroup* source = Cast<DockTabGroup>(objectSystem.Resolve(request.sourceGroupId));
    DockTabGroup* target = Cast<DockTabGroup>(objectSystem.Resolve(request.targetGroupId));

    if (!source || !target || source == target)
    {
        pendingMove.reset();
        return;
    }

    Widget& root = uiSystem.GetRootWidget();

    if (!IsInRoot(*source, root) || !IsInRoot(*target, root) || !source->HasTab(request.panelId))
    {
        pendingMove.reset();
        return;
    }

    std::optional<DetachedTab> detached = source->DetachTab(request.panelId);

    if (!detached.has_value())
        return;

    target->AddTab(detached->title, std::move(detached->panel));
    pendingMove.reset();
}

} // namespace URay
