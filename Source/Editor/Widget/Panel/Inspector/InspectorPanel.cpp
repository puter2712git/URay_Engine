#include "InspectorPanel.h"

#include <Editor/Widget/EditorWidgetFactory.h>
#include <Editor/Widget/Style/EditorStyles.h>

#include <Core/Math/Color.h>
#include <Core/Math/Quaternion.h>
#include <Core/Math/Vector2.h>
#include <Core/Math/Vector3.h>
#include <Core/Math/Vector4.h>
#include <Editor/Editor.h>
#include <Editor/Selection/SelectionSystem.h>
#include <Editor/Widget/Element/AssetField.h>
#include <Engine/UI/Widget/Element/Button.h>
#include <Engine/UI/Widget/Element/Checkbox.h>
#include <Editor/Widget/Element/ColorField.h>
#include <Editor/Widget/Element/Float2Field.h>
#include <Editor/Widget/Element/Float3Field.h>
#include <Editor/Widget/Element/Float4Field.h>
#include <Editor/Widget/Element/IntField.h>
#include <Editor/Widget/Element/NumberField.h>
#include <Editor/Widget/Element/Separator.h>
#include <Editor/Widget/Element/StringField.h>
#include <Editor/Widget/Layer/PopupLayer.h>
#include <Engine/UI/Widget/Layout/VerticalLayout.h>
#include <Editor/Widget/Panel/Inspector/ComponentSection.h>
#include <Editor/Widget/Panel/Inspector/InspectSystem.h>
#include <Editor/Widget/Panel/Inspector/PropertyRow.h>
#include <Editor/Widget/Panel/ScrollView.h>
#include <Editor/Widget/Popup/ComponentPickerPopup.h>
#include <Editor/Widget/EditorUI.h>
#include <Engine/Asset/Asset.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Material/Material.h>
#include <Engine/Asset/Mesh/Mesh.h>
#include <Engine/Asset/Mesh/SkinnedMesh.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Engine/Component/Component.h>
#include <Engine/Engine.h>
#include <Engine/Object/Object.h>
#include <Engine/Object/Property/Property.h>
#include <Engine/Scene/SceneSystem.h>
#include <Engine/Scene/Unit.h>

#include <variant>

namespace URay
{

InspectorPanel::InspectorPanel() : Panel("Inspector"), inspectSystem(gEditor->GetInspectSystem())
{
    SetBackgroundColor(EditorStyle::Surface);
    SetOutlineWidth(2.0f);
    SetOutlineColor(EditorStyle::Border);
    SetPadding(Padding(5.0f, 5.0f));

    scrollView = AddChild<ScrollView>();

    layout = scrollView->SetContent<VerticalLayout>();
    layout->SetGap(4.0f);
    layout->SetCrossAxisAlignment(CrossAxisAlignment::Stretch);

    inspectSystem.GetOnTargetChangedRay().Register(
        this,
        [this](const InspectTarget&, const InspectTarget&)
        { isDirty = true; });
}

InspectorPanel::~InspectorPanel()
{
    inspectSystem.GetOnTargetChangedRay().UnregisterAll(this);
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

    const InspectTarget& target = inspectSystem.GetTarget();

    if (const auto* unitTarget = std::get_if<UnitInspectTarget>(&target.data))
    {
        if (unitTarget->unit)
            RebuildUnitProperties(*unitTarget->unit);

        return;
    }

    if (const auto* assetTarget = std::get_if<AssetInspectTarget>(&target.data))
    {
        RebuildAssetProperties(assetTarget->handle);
        return;
    }
}

void InspectorPanel::RebuildUnitProperties(Unit& unit)
{
    ComponentSection* unitSection = layout->AddChild<ComponentSection>("Unit");

    for (const Property& property : unit.GetClass()->GetAllProperties())
    {
        AddProperty(*unitSection->GetContent(), unit, property, property.name);
    }

    layout->AddChild<Separator>(SeparatorOrientation::Horizontal);

    const auto& components = unit.GetComponents();
    for (const auto& comp : components)
    {
        ComponentSection* section = layout->AddChild<ComponentSection>(
            std::string(comp->GetClass()->GetType().name));

        for (const Property& property : comp->GetClass()->GetAllProperties())
        {
            AddProperty(*section->GetContent(), *comp, property, property.name);
        }
    }

    layout->AddChild<Separator>(SeparatorOrientation::Horizontal);

    Unit* inspectedUnit = &unit;

    Button* addComponentButton = CreateEditorButton(*layout, "Add Component");
    addComponentButton->SetPreferredHeight(26.0f);
    addComponentButton->GetOnClickRay().Register(
        this,
        [this, addComponentButton, inspectedUnit]
        {
            PopupLayer& popupLayer = gEditor->GetEditorUI().GetPopupLayer();

            ComponentPickerPopup* popup = popupLayer.OpenPopup<ComponentPickerPopup>(addComponentButton->GetRect(), *inspectedUnit);
            popup->GetComponentSelectedRay().Register(
                popup,
                [this, &popupLayer, inspectedUnit](const ComponentPickerSelection& selection)
                {
                    auto component = ComponentFactory::Create(selection.componentTypeName);
                    if (component)
                    {
                        inspectedUnit->AddComponent(std::move(component));
                    }

                    popupLayer.ClosePopup();
                });
        });
}

void InspectorPanel::RebuildAssetProperties(AssetHandle handle)
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    Asset* asset = assetDatabase.Find<Asset>(handle);

    if (!asset)
        return;

    ComponentSection* section = layout->AddChild<ComponentSection>(asset->GetName());

    for (const Property& property : asset->GetClass()->GetAllProperties())
    {
        AddProperty(*section->GetContent(), *asset, property, property.name);
    }
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

    case PropertyType::Rotation:
        AddRotationProperty(targetLayout, object, property, name);
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
    case PropertyType::SkinnedMesh:
    case PropertyType::Texture:
    case PropertyType::Material:
        AddAssetProperty(targetLayout, object, property, name);
        break;

    default:
        break;
    }
}

void InspectorPanel::AddBoolProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name)
{
    bool* value = reinterpret_cast<bool*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    Checkbox* checkbox = row->SetValueWidget<Checkbox>();
    ConfigureEditorCheckbox(*checkbox);
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

void InspectorPanel::AddRotationProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name)
{
    Quaternion* value = reinterpret_cast<Quaternion*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    Float3Field* field = row->SetValueWidget<Float3Field>();
    field->SetValue(value->ToEulerDegrees());

    field->GetValueChangedRay().Register(
        this,
        [&object, property, value](const Vector3& newValue)
        {
            *value = Quaternion::FromEulerDegrees(newValue);
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
}

void InspectorPanel::AddAssetProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name)
{
    Class* assetClass = nullptr;

    switch (property.type)
    {
    case PropertyType::Mesh:
        assetClass = Mesh::StaticClass();
        break;

    case PropertyType::SkinnedMesh:
        assetClass = SkinnedMesh::StaticClass();
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
