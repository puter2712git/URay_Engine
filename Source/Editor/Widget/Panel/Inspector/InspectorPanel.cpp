#include "InspectorPanel.h"

#include <Editor/Editor.h>
#include <Editor/Selection/SelectionSystem.h>
#include <Editor/Widget/Element/Checkbox.h>
#include <Editor/Widget/Element/ColorField.h>
#include <Editor/Widget/Element/Float3Field.h>
#include <Editor/Widget/Element/NumberField.h>
#include <Editor/Widget/Element/Separator.h>
#include <Editor/Widget/Element/StringField.h>
#include <Editor/Widget/Layout/VerticalLayout.h>
#include <Editor/Widget/Panel/Inspector/PropertyRow.h>
#include <Editor/Widget/Panel/ScrollView.h>

#include <Engine/Component/Component.h>
#include <Engine/Object/Object.h>
#include <Engine/Object/Property/Property.h>
#include <Engine/Scene/Unit.h>

#include <Core/Math/Color.h>
#include <Core/Math/Vector3.h>

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

    scrollView = AddChild<ScrollView>();

    layout = scrollView->SetContent<VerticalLayout>();
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
    scrollView->Arrange(GetContentRect());
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
        AddProperty(*selectedUnit, property, property.name);
    }

    layout->AddChild<Separator>(SeparatorOrientation::Horizontal);

    const auto& components = selectedUnit->GetComponents();
    for (const auto& comp : components)
    {
        const std::string componentName(comp->GetClass()->GetType().name);
        for (const Property& property : comp->GetClass()->GetAllProperties())
        {
            AddProperty(*comp, property, componentName + "." + property.name);
        }

        layout->AddChild<Separator>(SeparatorOrientation::Horizontal);
    }
}

void InspectorPanel::AddProperty(Object& object, const Property& property, const std::string& name)
{
    switch (property.type)
    {
    case PropertyType::Bool:
        AddBoolProperty(object, property, name);
        break;

    case PropertyType::Float:
        AddFloatProperty(object, property, name);
        break;

    case PropertyType::Vector3:
        AddVector3Property(object, property, name);
        break;

    case PropertyType::Color:
        AddColorProperty(object, property, name);
        break;

    case PropertyType::String:
        AddStringProperty(object, property, name);
        break;

    default:
        break;
    }
}

void InspectorPanel::AddBoolProperty(Object& object, const Property& property, const std::string& name)
{
    bool* value = reinterpret_cast<bool*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = layout->AddChild<PropertyRow>(name);

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

void InspectorPanel::AddFloatProperty(Object& object, const Property& property, const std::string& name)
{
    float* value = reinterpret_cast<float*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = layout->AddChild<PropertyRow>(name);

    NumberField* field = row->SetValueWidget<NumberField>();
    field->SetValue(*value);

    field->GetValueChangedRay().Register(
        this,
        [&object, property, value, field](float newValue)
        {
            *value = newValue;
            object.NotifyPropertyChanged(property);

            if (property.OnChangedCallback)
                property.OnChangedCallback(object, property);
        });
}

void InspectorPanel::AddVector3Property(Object& object, const Property& property, const std::string& name)
{
    Vector3* value = reinterpret_cast<Vector3*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = layout->AddChild<PropertyRow>(name);

    Float3Field* field = row->SetValueWidget<Float3Field>();
    field->SetValue(*value);

    field->GetValueChangedRay().Register(
        this,
        [&object, property, value](const Vector3& newValue)
        {
            *value = newValue;
            object.NotifyPropertyChanged(property);

            if (property.OnChangedCallback)
                property.OnChangedCallback(object, property);
        });
}

void InspectorPanel::AddColorProperty(Object& object, const Property& property, const std::string& name)
{
    Color* value = reinterpret_cast<Color*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = layout->AddChild<PropertyRow>(name);

    ColorField* field = row->SetValueWidget<ColorField>();
    field->SetValue(*value);

    field->GetValueChangedRay().Register(
        this,
        [&object, property, value](const Color& newValue)
        {
            *value = newValue;
            object.NotifyPropertyChanged(property);

            if (property.OnChangedCallback)
                property.OnChangedCallback(object, property);
        });
}

void InspectorPanel::AddStringProperty(Object& object, const Property& property, const std::string& name)
{
    std::string* value = reinterpret_cast<std::string*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = layout->AddChild<PropertyRow>(name);

    StringField* field = row->SetValueWidget<StringField>();
    field->SetText(*value);

    field->GetValueChangedRay().Register(
        this,
        [&object, property, value, field]()
        {
            *value = field->GetText();
            object.NotifyPropertyChanged(property);

            if (property.OnChangedCallback)
                property.OnChangedCallback(object, property);
        });
}

} // namespace URay
