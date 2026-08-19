#include "InspectorWidget.h"

#include "Editor/Editor.h"
#include "Editor/PropertyDrawer.h"

#include "Engine/Component/Component.h"
#include "Engine/Unit.h"

#include <imgui/imgui.h>

namespace URay
{

InspectorWidget::InspectorWidget(Editor& editor)
    : editor(editor)
{
}

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

    Unit* selectedUnit = editor.GetSelectedUnit();

    if (!selectedUnit)
    {
        ImGui::End();
        return;
    }

    Class* cls = nullptr;
    std::vector<Property> properties;

    cls = selectedUnit->GetClass();
    properties = cls->GetProperties();

    for (Property& prop : properties)
    {
        ImGui::PushID(prop.name.c_str());

        PropertyDrawer::Draw(prop, selectedUnit);

        ImGui::PopID();
    }

    auto components = selectedUnit->GetComponents();
    for (Component* comp : components)
    {
        cls = comp->GetClass();
        properties = cls->GetProperties();

        ImGui::PushID(comp);

        ImGui::Text("%s", cls->GetName().c_str());

        for (Property& prop : properties)
        {
            ImGui::PushID(prop.name.c_str());

            PropertyDrawer::Draw(prop, comp);

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
                Component* newComp = constructor();
                selectedUnit->AddComponent(newComp);
            }
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}

} // namespace URay
