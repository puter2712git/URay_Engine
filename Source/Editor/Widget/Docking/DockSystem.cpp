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

void DockSystem::RequestMove(ObjectId sourceGroupId, ObjectId targetGroupId, ObjectId panelId, DockDropZone zone)
{
    pendingMove = MoveRequest{
        .sourceGroupId = sourceGroupId,
        .targetGroupId = targetGroupId,
        .panelId = panelId,
        .zone = zone
    };
}

void DockSystem::RequestDrop(ObjectId sourceGroupId, ObjectId panelId, const Vector2& position)
{
    const std::optional<DockDropTarget> target = FindDockDropTarget(sourceGroupId, position);

    ClearDropHighlight();

    if (!target.has_value())
        return;

    RequestMove(sourceGroupId, target->groupId, panelId, target->zone);
}

void DockSystem::UpdateDrag(ObjectId sourceGroupId, ObjectId panelId, const Vector2& position)
{
    DockTabGroup* source = Cast<DockTabGroup>(objectSystem.Resolve(sourceGroupId));

    if (!source || !source->HasTab(panelId))
    {
        ClearDropHighlight();
        return;
    }

    const std::optional<DockDropTarget> target = FindDockDropTarget(sourceGroupId, position);

    if (target.has_value() && highlightedTarget.has_value() && target->groupId == highlightedTarget->groupId && target->zone == highlightedTarget->zone)
        return;

    ClearDropHighlight();

    if (!target.has_value())
        return;

    DockTabGroup* targetGroup = Cast<DockTabGroup>(objectSystem.Resolve(target->groupId));

    if (!targetGroup)
        return;

    targetGroup->SetDropHighlight(target->zone);
    highlightedTarget = *target;
}

void DockSystem::ProcessPendingMove()
{
    if (!pendingMove.has_value())
        return;

    const MoveRequest request = *pendingMove;

    DockTabGroup* source = Cast<DockTabGroup>(objectSystem.Resolve(request.sourceGroupId));
    DockTabGroup* target = Cast<DockTabGroup>(objectSystem.Resolve(request.targetGroupId));

    if (!source || !target)
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
    const bool isCenter = request.zone == DockDropZone::Center;

    if (isCenter && source == target)
    {
        pendingMove.reset();
        return;
    }

    if (!isCenter)
    {
        if (source == target && source->GetTabCount() <= 1)
        {
            pendingMove.reset();
            return;
        }

        if (!CanSplitGroup(*target, request.zone))
        {
            pendingMove.reset();
            return;
        }

        if (!uiSystem.TryClearInputStateForSubtree(*source))
            return;

        std::unique_ptr<Widget> newGroup =
            std::make_unique<DockTabGroup>(uiSystem);

        DockTabGroup* newTabGroup =
            static_cast<DockTabGroup*>(newGroup.get());

        const ObjectId newGroupId = newTabGroup->GetId();

        if (!SplitGroup(*target, request.zone, newGroup))
        {
            pendingMove.reset();
            return;
        }

        std::optional<DetachedTab> detached =
            source->DetachTab(request.panelId);

        if (!detached.has_value())
        {
            if (!TryCleanupEmptyGroup(newGroupId))
                pendingCleanups.push_back(newGroupId);

            pendingMove.reset();
            return;
        }

        newTabGroup->AddTab(
            detached->title,
            std::move(detached->panel));
    }
    else
    {
        std::optional<DetachedTab> detached =
            source->DetachTab(request.panelId);

        if (!detached.has_value())
            return;

        target->AddTab(
            detached->title,
            std::move(detached->panel));
    }

    if (source->IsEmpty())
    {
        const ObjectId sourceId = source->GetId();

        const auto it = std::find(
            pendingCleanups.begin(),
            pendingCleanups.end(),
            sourceId);

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

bool DockSystem::SplitGroup(DockTabGroup& target, DockDropZone zone, std::unique_ptr<Widget>& newGroup)
{
    if (!CanSplitGroup(target, zone))
        return false;

    if (!newGroup || newGroup->GetParent())
        return false;

    if (!Cast<DockTabGroup>(newGroup.get()))
        return false;

    if (newGroup.get() == &target)
        return false;

    SplitAxis axis;
    bool newGroupFirst;

    switch (zone)
    {
    case DockDropZone::Left:
        axis = SplitAxis::Horizontal;
        newGroupFirst = true;
        break;

    case DockDropZone::Right:
        axis = SplitAxis::Horizontal;
        newGroupFirst = false;
        break;

    case DockDropZone::Top:
        axis = SplitAxis::Vertical;
        newGroupFirst = true;
        break;

    case DockDropZone::Bottom:
        axis = SplitAxis::Vertical;
        newGroupFirst = false;
        break;

    case DockDropZone::Center:
    default:
        return false;
    }

    Widget& root = uiSystem.GetRootWidget();

    if (!IsInRoot(target, root))
        return false;

    Widget* parent = target.GetParent();
    if (!parent)
        return false;

    if (parent == &root)
    {
        const auto& children = parent->GetChildren();

        if (children.empty() || children.front().get() != &target)
            return false;
    }
    else if (!Cast<Splitter>(parent))
    {
        return false;
    }

    std::unique_ptr<Widget> wrapper = std::make_unique<Splitter>("", axis);

    Splitter* splitter = static_cast<Splitter*>(wrapper.get());

    std::unique_ptr<Widget> existing = parent->ReplaceChild(&target, wrapper);

    if (!existing)
        return false;

    if (newGroupFirst)
    {
        splitter->AddChild(std::move(newGroup));
        splitter->AddChild(std::move(existing));
    }
    else
    {
        splitter->AddChild(std::move(existing));
        splitter->AddChild(std::move(newGroup));
    }

    return true;
}

bool DockSystem::CanSplitGroup(const DockTabGroup& target, DockDropZone zone) const
{
    constexpr float minGroupExtent = 100.0f;
    constexpr float splitterExtent = 4.0f;

    const Rect& targetRect = target.GetRect();

    switch (zone)
    {
    case DockDropZone::Left:
    case DockDropZone::Right:
        if (targetRect.size.x < minGroupExtent * 2.0f + splitterExtent)
            return false;
        break;

    case DockDropZone::Top:
    case DockDropZone::Bottom:
        if (targetRect.size.y < minGroupExtent * 2.0f + splitterExtent)
            return false;
        break;

    case DockDropZone::Center:
    default:
        return false;
    }

    Widget& root = uiSystem.GetRootWidget();

    if (!IsInRoot(target, root))
        return false;

    Widget* parent = target.GetParent();
    if (!parent)
        return false;

    if (parent == &root)
    {
        const auto& children = parent->GetChildren();

        return !children.empty() && children.front().get() == &target;
    }

    return Cast<Splitter>(parent) != nullptr;
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

        if (*zone != DockDropZone::Center && !CanSplitGroup(*target, *zone))
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

void DockSystem::ClearDropHighlight()
{
    if (!highlightedTarget.has_value())
        return;

    DockTabGroup* previous = Cast<DockTabGroup>(objectSystem.Resolve(highlightedTarget->groupId));

    if (previous)
        previous->SetDropHighlight(std::nullopt);

    highlightedTarget.reset();
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
