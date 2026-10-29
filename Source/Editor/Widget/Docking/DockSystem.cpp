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

void DockSystem::RequestDrop(ObjectId sourceGroupId, ObjectId panelId, const Vector2& position)
{
    DockTabGroup* target = FindDropTarget(sourceGroupId, position);

    ClearDropHighlight();

    if (target)
        RequestMove(sourceGroupId, target->GetId(), panelId);
}

void DockSystem::UpdateDrag(ObjectId sourceGroupId, ObjectId panelId, const Vector2& position)
{
    DockTabGroup* source = Cast<DockTabGroup>(objectSystem.Resolve(sourceGroupId));

    if (!source || !source->HasTab(panelId))
    {
        ClearDropHighlight();
        return;
    }

    DockTabGroup* target = FindDropTarget(sourceGroupId, position);

    if (target && highlightedGroupId.has_value() && *highlightedGroupId == target->GetId())
        return;

    ClearDropHighlight();

    if (target)
    {
        highlightedGroupId = target->GetId();
        target->SetDropHighlighted(true);
    }
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

DockTabGroup* DockSystem::FindDropTarget(ObjectId sourceGroupId, const Vector2& position) const
{
    Widget* hit = uiSystem.FindWidgetAt(position);

    for (Widget* current = hit; current; current = current->GetParent())
    {
        DockTabGroup* target = Cast<DockTabGroup>(current);
        if (!target)
            continue;

        if (target->GetId() == sourceGroupId)
            return nullptr;

        return target;
    }

    return nullptr;
}

void DockSystem::ClearDropHighlight()
{
    if (!highlightedGroupId.has_value())
        return;

    DockTabGroup* previous = Cast<DockTabGroup>(objectSystem.Resolve(*highlightedGroupId));

    if (previous)
        previous->SetDropHighlighted(false);

    highlightedGroupId.reset();
}

} // namespace URay
