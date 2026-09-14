#include "SceneTreeWidget.h"

#include "Editor/Editor.h"
#include "Editor/Selection/SelectionSystem.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

#include <imgui/imgui.h>

namespace URay
{

namespace
{

const char* GetSceneLabel(SceneType type)
{
    switch (type)
    {
    case SceneType::Game:
        return "Game Scene";
    case SceneType::Editor:
        return "Editor Scene";
    case SceneType::Play:
        return "Play Scene";
    }

    return "Unknown Scene";
}

} // namespace

SceneTreeWidget::SceneTreeWidget() : selectionSystem(gEditor->GetSelectionSystem()) {}

SceneTreeWidget::~SceneTreeWidget() = default;

EventReply SceneTreeWidget::OnPointerDown(const PointerEvent& event)
{
    return EventReply{
        .requestFocus = true,
    };
}

void SceneTreeWidget::OnDraw()
{
    ApplyRect();
    ImGui::Begin("Scene Tree", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    const auto& scenes = sceneSystem.GetScenes();

    if (scenes.empty())
    {
        ImGui::End();
        return;
    }

    Unit* currSelectedUnit = selectionSystem.GetSelectedUnit();

    pendingDraggedUnit = nullptr;
    pendingParentUnit = nullptr;

    for (const std::unique_ptr<Scene>& scene : scenes)
    {
        if (scene)
        {
            DrawScene(*scene, currSelectedUnit);
        }
    }

    if (pendingDraggedUnit && pendingParentUnit &&
        pendingDraggedUnit->GetOwner() == pendingParentUnit->GetOwner())
    {
        pendingDraggedUnit->SetParent(pendingParentUnit);
    }

    ImGui::End();
}

void SceneTreeWidget::DrawScene(Scene& scene, Unit* currSelectedUnit)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

    bool hasRootUnit = false;
    for (const auto& unit : scene.GetUnits())
    {
        if (unit && !unit->GetParent())
        {
            hasRootUnit = true;
            break;
        }
    }

    if (!hasRootUnit)
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    const bool opened = ImGui::TreeNodeEx(&scene, flags, "%s", GetSceneLabel(scene.GetType()));

    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Create Empty"))
        {
            std::unique_ptr<Unit> newUnit = std::make_unique<Unit>();
            newUnit->SetName("New Unit");
            scene.AddUnit(std::move(newUnit));
        }

        if (ImGui::MenuItem("Create Empty (with Transform)"))
        {
            std::unique_ptr<Unit> newUnit = std::make_unique<Unit>();
            newUnit->SetName("New Unit");

            std::unique_ptr<TransformComponent> transform = std::make_unique<TransformComponent>();
            newUnit->AddComponent(std::move(transform));
            scene.AddUnit(std::move(newUnit));
        }

        ImGui::EndPopup();
    }

    if (!opened)
        return;

    for (const auto& unit : scene.GetUnits())
    {
        if (unit && !unit->GetParent())
        {
            DrawUnit(unit.get(), currSelectedUnit);
        }
    }

    ImGui::TreePop();
}

void SceneTreeWidget::DrawUnit(Unit* unit, Unit* currSelectedUnit)
{
    if (!unit)
        return;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
    if (unit == currSelectedUnit)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (unit->GetChildren().empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    bool opened = ImGui::TreeNodeEx(unit, flags, "%s", unit->GetName().c_str());

    DrawUnitDragDrop(unit);

    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    {
        selectionSystem.SelectUnit(unit);
    }

    if (opened)
    {
        for (Unit* child : unit->GetChildren())
        {
            DrawUnit(child, currSelectedUnit);
        }

        ImGui::TreePop();
    }
}

void SceneTreeWidget::DrawUnitDragDrop(Unit* unit)
{
    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("SCENE_TREE_UNIT", &unit, sizeof(unit));
        ImGui::Text("%s", unit->GetName().c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_TREE_UNIT"))
        {
            Unit* draggedUnit = *static_cast<Unit* const*>(payload->Data);
            if (draggedUnit)
            {
                pendingDraggedUnit = draggedUnit;
                pendingParentUnit = unit;
            }
        }

        ImGui::EndDragDropTarget();
    }
}

} // namespace URay
