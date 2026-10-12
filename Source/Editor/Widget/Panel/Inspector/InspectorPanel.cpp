#include "InspectorPanel.h"

#include <Core/Math/Color.h>
#include <Core/Math/Vector2.h>
#include <Core/Math/Vector3.h>
#include <Core/Math/Vector4.h>
#include <Editor/Editor.h>
#include <Editor/Selection/SelectionSystem.h>
#include <Editor/Widget/Element/AssetField.h>
#include <Editor/Widget/Element/Button.h>
#include <Editor/Widget/Element/Checkbox.h>
#include <Editor/Widget/Element/ColorField.h>
#include <Editor/Widget/Element/Float2Field.h>
#include <Editor/Widget/Element/Float3Field.h>
#include <Editor/Widget/Element/Float4Field.h>
#include <Editor/Widget/Element/IntField.h>
#include <Editor/Widget/Element/NumberField.h>
#include <Editor/Widget/Element/Separator.h>
#include <Editor/Widget/Element/StringField.h>
#include <Editor/Widget/Layer/PopupLayer.h>
#include <Editor/Widget/Layout/VerticalLayout.h>
#include <Editor/Widget/Panel/Inspector/ComponentSection.h>
#include <Editor/Widget/Panel/Inspector/PropertyRow.h>
#include <Editor/Widget/Panel/ScrollView.h>
#include <Editor/Widget/Popup/ComponentPickerPopup.h>
#include <Editor/Widget/WidgetSystem.h>
#include <Engine/Asset/Material/Material.h>
#include <Engine/Asset/Mesh/Mesh.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Engine/Component/Component.h>
#include <Engine/Component/ScriptComponent.h>
#include <Engine/Engine.h>
#include <Engine/Object/Object.h>
#include <Engine/Object/Property/Property.h>
#include <Engine/Scene/SceneSystem.h>
#include <Engine/Scene/Unit.h>

#include <variant>

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
    layout->SetGap(4.0f);
    layout->SetCrossAxisAlignment(CrossAxisAlignment::Stretch);

    selectionSystem.GetOnSelectionChangedRay().Register(
        this,
        [this](Unit*, Unit*)
        { isDirty = true; });

    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    sceneSystem.GetComponentAddRay().Register(
        this,
        [this](Scene*, Unit* unit, Component*)
        {
            if (unit == selectionSystem.GetSelectedUnit())
            {
                isDirty = true;
            }
        });
}

InspectorPanel::~InspectorPanel()
{
    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    sceneSystem.GetComponentAddRay().UnregisterAll(this);

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

    ComponentSection* unitSection = layout->AddChild<ComponentSection>("Unit");

    for (const Property& property : selectedUnit->GetClass()->GetAllProperties())
    {
        AddProperty(*unitSection->GetContent(), *selectedUnit, property, property.name);
    }

    layout->AddChild<Separator>(SeparatorOrientation::Horizontal);

    const auto& components = selectedUnit->GetComponents();
    for (const auto& comp : components)
    {
        const std::string title(comp->GetClass()->GetType().name);

        ComponentSection* section = layout->AddChild<ComponentSection>(title);

        for (const Property& property : comp->GetClass()->GetAllProperties())
        {
            AddProperty(*section->GetContent(), *comp, property, property.name);
        }

        if (ScriptComponent* scriptComponent = Cast<ScriptComponent>(comp.get()))
        {
            for (const ScriptFieldValue& field : scriptComponent->GetFields())
            {
                AddScriptField(
                    *section->GetContent(),
                    *scriptComponent,
                    field);
            }
        }
    }

    layout->AddChild<Separator>(SeparatorOrientation::Horizontal);

    Button* addComponentButton = layout->AddChild<Button>("Add Component");
    addComponentButton->SetPreferredHeight(26.0f);
    addComponentButton->GetOnClickRay().Register(
        this,
        [this, addComponentButton]
        {
            Unit* selectedUnit = selectionSystem.GetSelectedUnit();
            if (!selectedUnit)
                return;

            PopupLayer& popupLayer = gEditor->GetWidgetSystem().GetPopupLayer();

            ComponentPickerPopup* popup = popupLayer.OpenPopup<ComponentPickerPopup>(addComponentButton->GetRect(), *selectedUnit);
            popup->GetComponentSelectedRay().Register(
                popup,
                [this, &popupLayer](const std::string& typeName)
                {
                    Unit* targetUnit = selectionSystem.GetSelectedUnit();

                    if (targetUnit)
                    {
                        if (auto component = ComponentFactory::Create(typeName))
                        {
                            targetUnit->AddComponent(std::move(component));
                        }
                    }

                    popupLayer.ClosePopup();
                });
        });
}

void InspectorPanel::AddProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name)
{
    switch (property.type)
    {
    case PropertyType::Bool:
        AddBoolProperty(targetLayout, object, property, name);
        break;

    case PropertyType::Int:
        AddIntProperty(targetLayout, object, property, name);
        break;

    case PropertyType::Float:
        AddFloatProperty(targetLayout, object, property, name);
        break;

    case PropertyType::Vector2:
        AddVector2Property(targetLayout, object, property, name);
        break;

    case PropertyType::Vector3:
        AddVector3Property(targetLayout, object, property, name);
        break;

    case PropertyType::Vector4:
        AddVector4Property(targetLayout, object, property, name);
        break;

    case PropertyType::Color:
        AddColorProperty(targetLayout, object, property, name);
        break;

    case PropertyType::String:
        AddStringProperty(targetLayout, object, property, name);
        break;

    case PropertyType::Mesh:
    case PropertyType::Texture:
    case PropertyType::Material:
        AddAssetProperty(targetLayout, object, property, name);
        break;

    default:
        break;
    }
}

void InspectorPanel::AddScriptField(
    VerticalLayout& targetLayout,
    ScriptComponent& component,
    const ScriptFieldValue& field)
{
    const std::string fieldName = field.name;
    ScriptComponent* const scriptComponent = &component;

    switch (field.type)
    {
    case PropertyType::Bool:
    {
        const bool* value = std::get_if<bool>(&field.value);
        if (!value)
            return;

        PropertyRow* row =
            targetLayout.AddChild<PropertyRow>(fieldName);

        Checkbox* checkbox =
            row->SetValueWidget<Checkbox>();

        checkbox->SetChecked(*value);

        checkbox->GetValueChangeRay().Register(
            this,
            [scriptComponent, fieldName](bool newValue)
            {
                scriptComponent->SetFieldValue(
                    fieldName,
                    ScriptValue(newValue));
            });

        break;
    }

    case PropertyType::Int:
    {
        const int32* value = std::get_if<int32>(&field.value);
        if (!value)
            return;

        PropertyRow* row =
            targetLayout.AddChild<PropertyRow>(fieldName);

        IntField* intField =
            row->SetValueWidget<IntField>();

        intField->SetValue(*value);

        intField->GetValueChangedRay().Register(
            this,
            [scriptComponent, fieldName](int32 newValue)
            {
                scriptComponent->SetFieldValue(
                    fieldName,
                    ScriptValue(newValue));
            });

        break;
    }

    case PropertyType::Float:
    {
        const float* value = std::get_if<float>(&field.value);
        if (!value)
            return;

        PropertyRow* row =
            targetLayout.AddChild<PropertyRow>(fieldName);

        NumberField* numberField =
            row->SetValueWidget<NumberField>();

        numberField->SetValue(*value);

        numberField->GetValueChangedRay().Register(
            this,
            [scriptComponent, fieldName](float newValue)
            {
                scriptComponent->SetFieldValue(
                    fieldName,
                    ScriptValue(newValue));
            });

        break;
    }

    case PropertyType::Vector2:
    {
        const Vector2* value = std::get_if<Vector2>(&field.value);
        if (!value)
            return;

        PropertyRow* row =
            targetLayout.AddChild<PropertyRow>(fieldName);

        Float2Field* vectorField =
            row->SetValueWidget<Float2Field>();

        vectorField->SetValue(*value);

        vectorField->GetValueChangedRay().Register(
            this,
            [scriptComponent, fieldName](const Vector2& newValue)
            {
                scriptComponent->SetFieldValue(
                    fieldName,
                    ScriptValue(newValue));
            });

        break;
    }

    case PropertyType::Vector3:
    {
        const Vector3* value = std::get_if<Vector3>(&field.value);
        if (!value)
            return;

        PropertyRow* row =
            targetLayout.AddChild<PropertyRow>(fieldName);

        Float3Field* vectorField =
            row->SetValueWidget<Float3Field>();

        vectorField->SetValue(*value);

        vectorField->GetValueChangedRay().Register(
            this,
            [scriptComponent, fieldName](const Vector3& newValue)
            {
                scriptComponent->SetFieldValue(
                    fieldName,
                    ScriptValue(newValue));
            });

        break;
    }

    case PropertyType::Vector4:
    {
        const Vector4* value = std::get_if<Vector4>(&field.value);
        if (!value)
            return;

        PropertyRow* row =
            targetLayout.AddChild<PropertyRow>(fieldName);

        Float4Field* vectorField =
            row->SetValueWidget<Float4Field>();

        vectorField->SetValue(*value);

        vectorField->GetValueChangedRay().Register(
            this,
            [scriptComponent, fieldName](const Vector4& newValue)
            {
                scriptComponent->SetFieldValue(
                    fieldName,
                    ScriptValue(newValue));
            });

        break;
    }

    case PropertyType::Color:
    {
        const Color* value = std::get_if<Color>(&field.value);
        if (!value)
            return;

        PropertyRow* row =
            targetLayout.AddChild<PropertyRow>(fieldName);

        ColorField* colorField =
            row->SetValueWidget<ColorField>();

        colorField->SetValue(*value);

        colorField->GetValueChangedRay().Register(
            this,
            [scriptComponent, fieldName](const Color& newValue)
            {
                scriptComponent->SetFieldValue(
                    fieldName,
                    ScriptValue(newValue));
            });

        break;
    }

    case PropertyType::String:
    {
        const std::string* value =
            std::get_if<std::string>(&field.value);

        if (!value)
            return;

        PropertyRow* row =
            targetLayout.AddChild<PropertyRow>(fieldName);

        StringField* stringField =
            row->SetValueWidget<StringField>();

        stringField->SetText(*value);

        stringField->GetValueChangedRay().Register(
            this,
            [scriptComponent, fieldName, stringField]()
            {
                scriptComponent->SetFieldValue(
                    fieldName,
                    ScriptValue(stringField->GetText()));
            });

        break;
    }

    default:
        break;
    }
}

void InspectorPanel::AddBoolProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name)
{
    bool* value = reinterpret_cast<bool*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

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

void InspectorPanel::AddFloatProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name)
{
    float* value = reinterpret_cast<float*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

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

void InspectorPanel::AddIntProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name)
{
    int32* value = reinterpret_cast<int32*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    IntField* field = row->SetValueWidget<IntField>();
    field->SetValue(*value);

    field->GetValueChangedRay().Register(
        this,
        [&object, property, value](int32 newValue)
        {
            *value = newValue;
            object.NotifyPropertyChanged(property);

            if (property.OnChangedCallback)
                property.OnChangedCallback(object, property);
        });
}

void InspectorPanel::AddVector2Property(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name)
{
    Vector2* value = reinterpret_cast<Vector2*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    Float2Field* field = row->SetValueWidget<Float2Field>();
    field->SetValue(*value);

    field->GetValueChangedRay().Register(
        this,
        [&object, property, value](const Vector2& newValue)
        {
            *value = newValue;
            object.NotifyPropertyChanged(property);

            if (property.OnChangedCallback)
                property.OnChangedCallback(object, property);
        });
}

void InspectorPanel::AddVector3Property(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name)
{
    Vector3* value = reinterpret_cast<Vector3*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

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

void InspectorPanel::AddColorProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name)
{
    Color* value = reinterpret_cast<Color*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

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

void InspectorPanel::AddVector4Property(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name)
{
    Vector4* value = reinterpret_cast<Vector4*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    Float4Field* field = row->SetValueWidget<Float4Field>();
    field->SetValue(*value);

    field->GetValueChangedRay().Register(
        this,
        [&object, property, value](const Vector4& newValue)
        {
            *value = newValue;
            object.NotifyPropertyChanged(property);

            if (property.OnChangedCallback)
                property.OnChangedCallback(object, property);
        });
}

void InspectorPanel::AddStringProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name)
{
    std::string* value = reinterpret_cast<std::string*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

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

    if (Cast<ScriptComponent>(&object) && property.name == "Script Class")
    {
        field->GetValueCommittedRay().Register(
            this,
            [this]()
            { isDirty = true; });
    }
}

void InspectorPanel::AddAssetProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name)
{
    Class* assetClass = nullptr;

    switch (property.type)
    {
    case PropertyType::Mesh:
        assetClass = Mesh::StaticClass();
        break;

    case PropertyType::Texture:
        assetClass = Texture::StaticClass();
        break;

    case PropertyType::Material:
        assetClass = Material::StaticClass();
        break;

    default:
        return;
    }

    auto* value = reinterpret_cast<AssetHandle*>(
        reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    AssetField* field = row->SetValueWidget<AssetField>(assetClass);
    field->SetValue(*value);

    field->GetValueChangedRay().Register(
        this,
        [&object, property, value](AssetHandle newValue)
        {
            if (*value == newValue)
                return;

            *value = newValue;
            object.NotifyPropertyChanged(property);

            if (property.OnChangedCallback)
                property.OnChangedCallback(object, property);
        });
}

} // namespace URay
