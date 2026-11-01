#include "SceneTreePanel.h"

#include <Editor/Widget/Panel/PanelFactory.h>

#include <Core/Math/Math.h>
#include <Editor/Widget/Style/EditorStyles.h>

#include <Editor/Editor.h>
#include <Editor/Selection/SelectionSystem.h>
#include <Editor/Widget/EditorUI.h>
#include <Editor/Widget/Layer/PopupLayer.h>
#include <Editor/Widget/Panel/SceneTree/SceneTreeNode.h>
#include <Editor/Widget/Panel/ScrollView.h>
#include <Editor/Widget/Popup/UnitPopup.h>
#include <Engine/Engine.h>
#include <Engine/Scene/SceneSystem.h>
#include <Engine/Scene/Unit.h>

#include <algorithm>
#include <cmath>

namespace URay
{

URAY_REGISTER_PANEL(SceneTreePanel, "Scene Tree")

namespace
{
constexpr float SceneTreeRowHeight = 20.0f;
constexpr int32 SceneTreeRowOverscan = 3;
} // namespace

class SceneTreeVirtualContent final : public Widget
{
public:
    explicit SceneTreeVirtualContent(SceneTreePanel& owner);

public:
    void Arrange(const Rect& rect) override;

protected:
    Vector2 OnMeasure() override;

private:
    SceneTreePanel& owner;
    std::vector<SceneTreeNode*> nodePool;
};

SceneTreePanel::SceneTreePanel()
{
    sceneSystem = &gEngine->GetSceneSystem();

    SetBackgroundColor(EditorStyle::Surface);
    SetPadding(Padding(5.0f, 5.0f));

    scrollView = AddChild<ScrollView>();
    virtualContent = scrollView->SetContent<SceneTreeVirtualContent>(*this);

    sceneSystem->Register("UnitAdded", this, &SceneTreePanel::OnUnitAdded);
    sceneSystem->Register("UnitRemoved", this, &SceneTreePanel::OnUnitRemoved);

    UpdateTree();
}

SceneTreePanel::~SceneTreePanel()
{
    sceneSystem->UnregisterAll(this);
}

void SceneTreePanel::Arrange(const Rect& rect)
{
    Panel::Arrange(rect);
    scrollView->Arrange(GetContentRect());
}

EventReply SceneTreePanel::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton == MouseButton::Left && dragState)
    {
        return EventReply{
            .handled = true,
            .requestFocus = true,
            .capturePointer = true
        };
    }

    if (event.changedButton != MouseButton::Right)
        return {};

    PopupLayer& popupLayer = gEditor->GetEditorUI().GetPopupLayer();
    UnitPopup* popup = popupLayer.OpenPopup<UnitPopup>(event.position);

    popup->Register("UnitSelected", gEditor, &Editor::OnUnitPopupSelected, false, ObjectId{});

    return EventReply{
        .handled = true,
        .requestFocus = true
    };
}

EventReply SceneTreePanel::OnPointerMove(const PointerEvent& event)
{
    if (!dragState)
        return {};

    constexpr float dragThreshold = 4.0f;

    const float dx = event.position.x - dragState->pressPosition.x;
    const float dy = event.position.y - dragState->pressPosition.y;

    if (!dragState->isDragging && dx * dx + dy * dy >= dragThreshold * dragThreshold)
    {
        dragState->isDragging = true;
    }

    dragState->pointerPosition = event.position;

    return EventReply{ .handled = true };
}

EventReply SceneTreePanel::OnPointerUp(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left || !dragState)
        return {};

    if (dragState->isDragging)
    {
        const DropTarget target = FindDropTarget(event.position);
        ApplyDrop(target);
    }

    dragState.reset();

    return EventReply{
        .handled = true,
        .releasePointer = true
    };
}

void SceneTreePanel::OnPointerCancel()
{
    dragState.reset();
}

void SceneTreePanel::BeginDrag(SceneId sceneId, const UUID& unitId, const Vector2& position)
{
    dragState = DragState{
        .sceneId = sceneId,
        .unitId = unitId,
        .pressPosition = position,
        .pointerPosition = position,
        .isDragging = false
    };
}

bool SceneTreePanel::IsUnitDropTarget(SceneId sceneId, const UUID& unitId) const
{
    if (!dragState || !dragState->isDragging)
        return false;

    const DropTarget target = FindDropTarget(dragState->pointerPosition);

    if (target.type != DropTargetType::Unit ||
        target.sceneId != sceneId ||
        target.unitId != unitId ||
        sceneId != dragState->sceneId)
    {
        return false;
    }

    Scene* scene = sceneSystem->GetScene(sceneId);
    if (!scene)
        return false;

    Unit* source = scene->FindUnit(dragState->unitId);
    Unit* candidate = scene->FindUnit(unitId);

    if (!source || !candidate || source->GetParent() == candidate)
        return false;

    for (Unit* ancestor = candidate; ancestor; ancestor = ancestor->GetParent())
    {
        if (ancestor == source)
            return false;
    }

    return true;
}

void SceneTreePanel::OnUpdate(float deltaTime)
{
    if (!isTreeDirty)
        return;

    isTreeDirty = false;

    UpdateTree();
}

void SceneTreePanel::UpdateTree()
{
    rows.clear();

    auto scenes = sceneSystem->GetScenes(SceneType::Game);
    Scene* gameScene = scenes.empty() ? nullptr : scenes[0];

    if (!gameScene)
        return;

    for (const auto& unit : gameScene->GetUnits())
    {
        if (!unit->GetParent())
            AddSubtree(*unit, 0);
    }
}

void SceneTreePanel::AddSubtree(const Unit& unit, uint32 depth)
{
    rows.push_back(TreeRow{
        .sceneId = unit.GetOwner()->GetId(),
        .unitId = unit.GetPersistentId(),
        .name = unit.GetName(),
        .depth = depth });

    for (Unit* child : unit.GetChildren())
    {
        if (child)
            AddSubtree(*child, depth + 1);
    }
}

void SceneTreePanel::OnUnitAdded(Scene* scene, Unit* unit)
{
    isTreeDirty = true;
}

void SceneTreePanel::OnUnitRemoved(Scene* scene, Unit* unit)
{
    isTreeDirty = true;
}

SceneTreePanel::DropTarget SceneTreePanel::FindDropTarget(const Vector2& position) const
{
    if (!scrollView->GetRect().Contains(position))
        return {};

    const float localY = position.y - virtualContent->GetRect().position.y;
    if (localY < 0.0f)
        return {};

    const int32 rowIndex = static_cast<int32>(Math::Floor(localY / SceneTreeRowHeight));

    if (rowIndex >= static_cast<int32>(rows.size()))
    {
        return DropTarget{
            .type = DropTargetType::Root
        };
    }

    const TreeRow& row = rows[rowIndex];

    return DropTarget{
        .type = DropTargetType::Unit,
        .sceneId = row.sceneId,
        .unitId = row.unitId
    };
}

void SceneTreePanel::ApplyDrop(const DropTarget& target)
{
    if (!dragState || target.type == DropTargetType::None)
        return;

    Scene* scene = sceneSystem->GetScene(dragState->sceneId);
    if (!scene)
        return;

    Unit* source = scene->FindUnit(dragState->unitId);
    if (!source)
        return;

    Unit* newParent = nullptr;

    if (target.type == DropTargetType::Unit)
    {
        if (target.sceneId != dragState->sceneId)
            return;

        newParent = scene->FindUnit(target.unitId);
        if (!newParent)
            return;
    }

    if (source->GetParent() == newParent)
        return;

    if (source->SetParent(newParent, ReparentMode::KeepWorld))
        isTreeDirty = true;
}

SceneTreeVirtualContent::SceneTreeVirtualContent(SceneTreePanel& owner) : owner(owner) {}

void SceneTreeVirtualContent::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    const int32 rowCount = static_cast<int32>(owner.rows.size());

    if (rowCount == 0)
    {
        for (SceneTreeNode* node : nodePool)
        {
            node->Unbind();
            node->Arrange({});
        }

        return;
    }

    const Rect viewportRect = GetParent()->GetRect();

    const int32 firstVisible = Math::Max(
        0,
        static_cast<int32>(Math::Floor((viewportRect.position.y - rect.position.y) / SceneTreeRowHeight)));

    const int32 lastVisibleExclusive = Math::Min(
        rowCount,
        static_cast<int32>(Math::Ceil((viewportRect.position.y + viewportRect.size.y - rect.position.y) / SceneTreeRowHeight)));

    const int32 firstRow = Math::Max(0, firstVisible - SceneTreeRowOverscan);
    const int32 lastRow = Math::Min(rowCount, lastVisibleExclusive + SceneTreeRowOverscan);
    const int32 requiredNodeCount = lastRow - firstRow;

    while (static_cast<int32>(nodePool.size()) < requiredNodeCount)
    {
        nodePool.push_back(AddChild<SceneTreeNode>(owner));
    }

    for (int32 poolIndex = 0; poolIndex < requiredNodeCount; ++poolIndex)
    {
        const int32 rowIndex = firstRow + poolIndex;
        const SceneTreePanel::TreeRow& row = owner.rows[rowIndex];

        SceneTreeNode* node = nodePool[poolIndex];
        node->Bind(row.sceneId, row.unitId, row.name, row.depth);

        Rect rowRect;
        rowRect.position = Vector2(rect.position.x, rect.position.y + static_cast<float>(rowIndex) * SceneTreeRowHeight);
        rowRect.size = Vector2(rect.size.x, SceneTreeRowHeight);

        node->Arrange(rowRect);
    }

    for (size_t index = requiredNodeCount; index < nodePool.size(); ++index)
    {
        SceneTreeNode* node = nodePool[index];
        node->Unbind();
        node->Arrange({});
    }
}

Vector2 SceneTreeVirtualContent::OnMeasure()
{
    return Vector2(
        0.0f,
        static_cast<float>(owner.rows.size()) * SceneTreeRowHeight);
}

} // namespace URay
