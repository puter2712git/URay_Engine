#include "SceneTreeWidget.h"

#include "Editor/Editor.h"

#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Unit.h"

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

void SceneTreeWidget::OnDraw()
{
    ImGui::Begin("Scene Tree");

    Scene* scene = engine.GetSceneByType(SceneType::Game);
    if (!scene)
    {
        ImGui::End();
        return;
    }

    Unit* currSelectedUnit = editor.GetSelectedUnit();

    for (Unit* unit : scene->GetUnits())
    {
        DrawUnit(unit, currSelectedUnit);
    }

    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("Create Empty"))
        {
            Unit* newUnit = new Unit();
            newUnit->SetName("New Unit");
            gEngine->SpawnUnit(newUnit);
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}

void SceneTreeWidget::DrawUnit(Unit* unit, Unit* currSelectedUnit) const
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

} // namespace URay
