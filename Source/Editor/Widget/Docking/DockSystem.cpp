#include "DockSystem.h"

#include <Core/Math/Math.h>
#include <Editor/Widget/Docking/DockDropTarget.h>
#include <Editor/Widget/Docking/DockTabGroup.h>
#include <Editor/Widget/Element/Splitter.h>
#include <Engine/Object/ObjectSystem.h>
#include <Engine/UI/UISystem.h>

#include <algorithm>
#include <optional>
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

std::optional<DockDropZone> FindDropZone(const DockTabGroup& group, const Vector2& position)
{
    const Rect& groupRect = group.GetRect();
    const Rect panelRect = group.GetPanelRect();

    if (groupRect.size.x <= 0.0f || groupRect.size.y <= 0.0f)
        return std::nullopt;

    // Tab bar
    if (position.y < panelRect.position.y)
        return DockDropZone::Center;

    if (panelRect.size.x <= 0.0f || panelRect.size.y <= 0.0f)
        return std::nullopt;

    const float x = (position.x - panelRect.position.x) / panelRect.size.x;
    const float y = (position.y - panelRect.position.y) / panelRect.size.y;

    if (x < 0.0f || x >= 1.0f || y < 0.0f || y >= 1.0f)
        return std::nullopt;

    constexpr float edgeFraction = 0.25f;

    const float left = x;
    const float right = 1.0f - x;
    const float top = y;
    const float bottom = 1.0f - y;

    const float nearest = Math::Min(
        Math::Min(left, right),
        Math::Min(top, bottom));

    if (nearest >= edgeFraction)
        return DockDropZone::Center;

    if (nearest == left)
        return DockDropZone::Left;

    if (nearest == right)
        return DockDropZone::Right;

    if (nearest == top)
        return DockDropZone::Top;

    return DockDropZone::Bottom;
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

    if (source->IsEmpty())
    {
        const ObjectId sourceId = source->GetId();

        const auto it = std::find(pendingCleanups.begin(), pendingCleanups.end(), sourceId);

        if (it == pendingCleanups.end())
            pendingCleanups.push_back(sourceId);
    }

    pendingMove.reset();
}

void DockSystem::ProcessPendingCleanup()
{
    std::erase_if(
        pendingCleanups,
        [this](ObjectId groupId)
        {
            return TryCleanupEmptyGroup(groupId);
        });
}

std::optional<DockDropTarget> DockSystem::FindDockDropTarget(ObjectId sourceGroupId, const Vector2& position) const
{
    Widget* hit = uiSystem.FindWidgetAt(position);

    for (Widget* current = hit; current; current = current->GetParent())
    {
        DockTabGroup* target = Cast<DockTabGroup>(current);
        if (!target)
            continue;

        const std::optional<DockDropZone> zone = FindDropZone(*target, position);

        if (!zone.has_value())
            return std::nullopt;

        const bool isSource = target->GetId() == sourceGroupId;

        if (isSource)
        {
            if (*zone == DockDropZone::Center)
                return std::nullopt;

            if (target->GetTabCount() <= 1)
                return std::nullopt;
        }

        return DockDropTarget{
            .groupId = target->GetId(),
            .zone = *zone
        };
    }

    return std::nullopt;
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

bool DockSystem::TryCleanupEmptyGroup(ObjectId groupId)
{
    DockTabGroup* emptyGroup = Cast<DockTabGroup>(objectSystem.Resolve(groupId));

    if (!emptyGroup || !emptyGroup->IsEmpty())
        return true;

    Widget& root = uiSystem.GetRootWidget();

    if (!IsInRoot(*emptyGroup, root))
        return true;

    Splitter* splitter = Cast<Splitter>(emptyGroup->GetParent());

    if (!splitter)
        return true;

    Widget* parent = splitter->GetParent();
    if (!parent)
        return true;

    const auto& splitterChildren = splitter->GetChildren();

    if (splitterChildren.size() != 2)
        return true;

    const bool emptyIsFirst = splitterChildren[0].get() == emptyGroup;
    const bool emptyIsSecond = splitterChildren[1].get() == emptyGroup;

    if (!emptyIsFirst && !emptyIsSecond)
        return true;

    Widget* sibling = emptyIsFirst
                          ? splitterChildren[1].get()
                          : splitterChildren[0].get();

    const auto& parentChildren = parent->GetChildren();

    const auto parentIt = std::find_if(
        parentChildren.begin(),
        parentChildren.end(),
        [splitter](const std::unique_ptr<Widget>& child)
        {
            return child.get() == splitter;
        });

    if (parentIt == parentChildren.end())
        return true;

    if (parent == &root && (parentChildren.empty() || parentChildren.front().get() != splitter))
        return true;

    if (!uiSystem.TryClearInputStateForWidget(*splitter))
        return false;

    if (!uiSystem.TryClearInputStateForSubtree(*emptyGroup))
        return false;

    std::unique_ptr<Widget> survivor = splitter->RemoveChild(sibling);
    if (!survivor)
        return false;

    std::unique_ptr<Widget> removedSplitter = parent->ReplaceChild(splitter, survivor);

    if (!removedSplitter)
    {
        if (emptyIsFirst)
        {
            splitter->AddChild(std::move(survivor));
        }
        else
        {
            std::unique_ptr<Widget> empty = splitter->RemoveChild(emptyGroup);

            splitter->AddChild(std::move(survivor));
            splitter->AddChild(std::move(empty));
        }

        return false;
    }

    return true;
}

} // namespace URay
