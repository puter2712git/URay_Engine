#include "SceneTreeWidget.h"

#include "Editor/Editor.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

#include <imgui/imgui.h>

namespace URay
{

SceneTreeWidget::SceneTreeWidget(Editor& editor, Engine& engine)
    : editor(editor), engine(engine)
{
}

SceneTreeWidget::~SceneTreeWidget()
{
}

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

    SceneSystem& sceneSystem = engine.GetSceneSystem();
    Scene* scene = sceneSystem.GetSceneByType(SceneType::Game);
    if (!scene)
    {
        ImGui::End();
        return;
    }

    Unit* currSelectedUnit = editor.GetSelectedUnit();

    pendingDraggedUnit = nullptr;
    pendingParentUnit = nullptr;

    std::vector<Unit*> rootUnits;
    for (Unit* unit : scene->GetUnits())
    {
        if (unit && !unit->GetParent())
        {
            rootUnits.push_back(unit);
        }
    }

    for (Unit* unit : rootUnits)
    {
        DrawUnit(unit, currSelectedUnit);
    }

    if (pendingDraggedUnit && pendingParentUnit)
    {
        pendingDraggedUnit->SetParent(pendingParentUnit);
    }

    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("Create Empty"))
        {
            Unit* newUnit = new Unit();
            newUnit->SetName("New Unit");
            scene->AddUnit(newUnit);
        }

        if (ImGui::MenuItem("Create Empty (with Transform)"))
        {
            Unit* newUnit = new Unit();
            newUnit->SetName("New Unit");
            newUnit->AddComponent(new TransformComponent());
            scene->AddUnit(newUnit);
        }

        ImGui::EndPopup();
    }

    ImGui::End();
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
        editor.SelectUnit(unit);
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
