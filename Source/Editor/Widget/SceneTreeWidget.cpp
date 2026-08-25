#include "SceneTreeWidget.h"

#include "Editor/Editor.h"

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

    const auto& units = scene->GetUnits();

    ImGuiListClipper clipper;
    clipper.Begin(static_cast<int>(units.size()));

    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
        {
            DrawUnit(units[i], currSelectedUnit);
        }
    }

    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("Create Empty"))
        {
            Unit* newUnit = new Unit();
            newUnit->SetName("New Unit");
            scene->AddUnit(newUnit);
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
