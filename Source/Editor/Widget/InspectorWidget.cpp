#include "InspectorWidget.h"

#include "Editor/Editor.h"
#include "Editor/PropertyDrawer.h"
#include "Editor/Selection/SelectionSystem.h"

#include "Engine/Component/Component.h"
#include "Engine/Scene/Unit.h"

#include <imgui/imgui.h>

namespace URay
{

InspectorWidget::InspectorWidget() : selectionSystem(gEditor->GetSelectionSystem()) {}

InspectorWidget::~InspectorWidget() = default;

EventReply InspectorWidget::OnPointerDown(const PointerEvent& event)
{
    return EventReply{
        .requestFocus = true,
    };
}

void InspectorWidget::OnDraw()
{
    ApplyRect();
    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    Unit* selectedUnit = selectionSystem.GetSelectedUnit();

    if (!selectedUnit)
    {
        ImGui::End();
        return;
    }

    Class* cls = nullptr;
    std::vector<Property> properties;

    cls = selectedUnit->GetClass();
    properties = cls->GetAllProperties();

    for (Property& prop : properties)
    {
        ImGui::PushID(prop.name.c_str());

        PropertyDrawer::Draw(prop, selectedUnit);

        ImGui::PopID();
    }

    const auto& components = selectedUnit->GetComponents();
    for (const auto& comp : components)
    {
        cls = comp->GetClass();
        properties = cls->GetAllProperties();

        ImGui::PushID(comp.get());

        ImGui::Text("%s", cls->GetName().c_str());

        for (Property& prop : properties)
        {
            ImGui::PushID(prop.name.c_str());

            PropertyDrawer::Draw(prop, comp.get());

            ImGui::PopID();
        }

        ImGui::PopID();
    }

    if (ImGui::BeginPopupContextWindow())
    {
        auto& components = ComponentFactory::GetRegisteredComponents();
        for (auto [name, constructor] : components)
        {
            std::string menuName = "Add " + name;
            if (ImGui::MenuItem(menuName.c_str()))
            {
                std::unique_ptr<Component> newComp = constructor();
                selectedUnit->AddComponent(std::move(newComp));
            }
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}

} // namespace URay
