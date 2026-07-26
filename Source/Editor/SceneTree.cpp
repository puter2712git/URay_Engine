#include "SceneTree.h"

#include "Editor.h"

#include "Engine/Engine.h"
#include "Engine/Scene.h"
#include "Engine/Unit.h"

#include <imgui/imgui.h>

namespace URay
{

void SceneTree::Draw() const
{
    ImGui::Begin("Scene Tree");

    Scene* scene = gEngine->GetScene();
    if (!scene)
    {
        ImGui::End();
        return;
    }

    Editor* editor = gEngine->GetEditor();
    Unit* currSelectedUnit = editor->GetSelectedUnit();

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

void SceneTree::DrawUnit(Unit* unit, Unit* currSelectedUnit) const
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
        Editor* editor = gEngine->GetEditor();
        editor->SelectUnit(unit);
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
