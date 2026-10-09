#include "InspectorPanel.h"

#include <Editor/Editor.h>
#include <Editor/Selection/SelectionSystem.h>
#include <Editor/Widget/Element/Checkbox.h>
#include <Editor/Widget/Layout/VerticalLayout.h>
#include <Editor/Widget/Panel/Inspector/PropertyRow.h>

#include <Engine/Component/Component.h>
#include <Engine/Object/Object.h>
#include <Engine/Object/Property/Property.h>
#include <Engine/Scene/Unit.h>

namespace URay
{

InspectorPanel::InspectorPanel()
    : Panel("Inspector"),
      selectionSystem(gEditor->GetSelectionSystem())
{
    SetBackgroundColor(EditorStyle::Surface);
    SetOutlineWidth(2.0f);
    SetOutlineColor(EditorStyle::Border);
    SetPadding(Padding(5.0f, 5.0f));

    layout = AddChild<VerticalLayout>();
    layout->SetGap(2.0f);
    layout->SetCrossAxisAlignment(CrossAxisAlignment::Stretch);

    selectionSystem.GetOnSelectionChangedRay().Register(
        this,
        [this](Unit*, Unit*)
        { isDirty = true; });
}

InspectorPanel::~InspectorPanel()
{
    selectionSystem.GetOnSelectionChangedRay().UnregisterAll(this);
}

void InspectorPanel::Arrange(const Rect& rect)
{
    Panel::Arrange(rect);
    layout->Arrange(GetContentRect());
}

EventReply InspectorPanel::OnPointerDown(const PointerEvent& event)
{
    return EventReply{
        .requestFocus = true,
    };
}

void InspectorPanel::OnUpdate(float)
{
    if (!isDirty)
        return;

    isDirty = false;
    RebuildProperties();
}

void InspectorPanel::RebuildProperties()
{
    layout->ClearChildren();

    Unit* selectedUnit = selectionSystem.GetSelectedUnit();
    if (!selectedUnit)
        return;

    for (const Property& property : selectedUnit->GetClass()->GetAllProperties())
    {
        AddBoolProperty(*selectedUnit, property, property.name);
    }

    const auto& components = selectedUnit->GetComponents();
    for (const auto& comp : components)
    {
        const std::string componentName(comp->GetClass()->GetType().name);
        for (const Property& property : comp->GetClass()->GetAllProperties())
        {
            AddBoolProperty(*comp, property, componentName + "." + property.name);
        }
    }
}

void InspectorPanel::AddBoolProperty(Object& object, const Property& property, const std::string& name)
{
    if (property.type != PropertyType::Bool)
        return;

    bool* value = reinterpret_cast<bool*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = layout->AddChild<PropertyRow>(name);
    row->SetDesiredSize(Vector2(0.0f, 18.0f));

    Checkbox* checkbox = row->SetValueWidget<Checkbox>();
    checkbox->SetChecked(*value);

    checkbox->GetValueChangeRay().Register(
        this,
        [&object, property, value](bool checked)
        {
            *value = checked;
            object.NotifyPropertyChanged(property);

            if (property.OnChangedCallback)
                property.OnChangedCallback(object, property);
        });
}

} // namespace URay
