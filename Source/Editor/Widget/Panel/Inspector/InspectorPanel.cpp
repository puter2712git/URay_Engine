#include "InspectorPanel.h"
#include <Core/Type/TypeAlias.h>

#include <Core/Math/Color.h>
#include <Core/Math/Quaternion.h>
#include <Core/Math/Vector2.h>
#include <Core/Math/Vector3.h>
#include <Core/Math/Vector4.h>
#include <Editor/Editor.h>
#include <Editor/Selection/SelectionSystem.h>
#include <Editor/Widget/EditorUI.h>
#include <Editor/Widget/EditorWidgetFactory.h>
#include <Editor/Widget/Element/AssetField.h>
#include <Editor/Widget/Element/ColorField.h>
#include <Editor/Widget/Element/NumberField.h>
#include <Editor/Widget/Element/Separator.h>
#include <Editor/Widget/Element/StringField.h>
#include <Editor/Widget/Element/VectorField.h>
#include <Editor/Widget/Layer/PopupLayer.h>
#include <Editor/Widget/Panel/Inspector/ComponentSection.h>
#include <Editor/Widget/Panel/Inspector/InspectSystem.h>
#include <Editor/Widget/Panel/Inspector/PropertyRow.h>
#include <Editor/Widget/Panel/ScrollView.h>
#include <Editor/Widget/Popup/ComponentPickerPopup.h>
#include <Editor/Widget/Style/EditorStyles.h>
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
#include <Engine/UI/Widget/Element/Button.h>
#include <Engine/UI/Widget/Element/Checkbox.h>
#include <Engine/UI/Widget/Layout/VerticalLayout.h>

#include <utility>
#include <variant>

namespace URay
{

namespace
{

template <typename T>
void UpdateInspectedProperty(T value, Object* object, const Property* property)
{
    if (!object || !property)
        return;

    // Notifications may cause the inspector to be rebuilt, so keep a local copy.
    const Property propertyCopy = *property;
    auto* target = reinterpret_cast<T*>(reinterpret_cast<uint8*>(object) + propertyCopy.offset);
    *target = std::move(value);
    object->NotifyPropertyChanged(propertyCopy);

    if (propertyCopy.OnChangedCallback)
        propertyCopy.OnChangedCallback(*object, propertyCopy);
}

} // namespace

InspectorPanel::InspectorPanel() : inspectSystem(gEditor->GetInspectSystem())
{
    SetBackgroundColor(EditorStyle::Surface);
    SetPadding(Padding(5.0f, 5.0f));

    scrollView = AddChild<ScrollView>();

    layout = scrollView->SetContent<VerticalLayout>();
    layout->SetGap(4.0f);
    layout->SetCrossAxisAlignment(CrossAxisAlignment::Stretch);

    inspectSystem.Register("TargetChanged", this, &InspectorPanel::OnTargetChanged);
    inspectSystem.Register("TargetContentChanged", this, &InspectorPanel::OnTargetContentChanged);
}

InspectorPanel::~InspectorPanel()
{
    inspectSystem.UnregisterAll(this);
    layout->ClearChildren();
    propertyCopies.clear();
}

void InspectorPanel::OnTargetChanged(const InspectTarget*, const InspectTarget*)
{
    isDirty = true;
}

void InspectorPanel::OnTargetContentChanged(const InspectTarget*)
{
    isDirty = true;
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
    propertyCopies.clear();

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
            String(comp->GetClass()->GetType().name));

        for (const Property& property : comp->GetClass()->GetAllProperties())
        {
            AddProperty(*section->GetContent(), *comp, property, property.name);
        }
    }

    layout->AddChild<Separator>(SeparatorOrientation::Horizontal);

    Unit* inspectedUnit = &unit;

    Button* addComponentButton = CreateEditorButton(*layout, "Add Component");
    addComponentButton->SetPreferredHeight(26.0f);
    addComponentButton->Register("Clicked", this, &InspectorPanel::OnAddComponentClicked, addComponentButton, inspectedUnit);
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

void InspectorPanel::AddProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name)
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

const Property* InspectorPanel::KeepProperty(const Property& property)
{
    propertyCopies.push_back(std::make_unique<Property>(property));
    return propertyCopies.back().get();
}

void InspectorPanel::AddBoolProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name)
{
    bool* value = reinterpret_cast<bool*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    Checkbox* checkbox = row->SetValueWidget<Checkbox>();
    ConfigureEditorCheckbox(*checkbox);
    checkbox->SetChecked(*value);

    checkbox->Register("ValueChanged", this, &InspectorPanel::OnBoolChanged, &object, KeepProperty(property));
}

void InspectorPanel::AddFloatProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name)
{
    float* value = reinterpret_cast<float*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    NumberField<float>* field = row->SetValueWidget<NumberField<float>>();
    field->SetValue(*value);

    field->Register("ValueChanged", this, &InspectorPanel::OnFloatChanged, &object, KeepProperty(property));
}

void InspectorPanel::AddIntProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name)
{
    int32* value = reinterpret_cast<int32*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    NumberField<int32>* field = row->SetValueWidget<NumberField<int32>>();
    field->SetValue(*value);

    field->Register("ValueChanged", this, &InspectorPanel::OnIntChanged, &object, KeepProperty(property));
}

void InspectorPanel::AddVector2Property(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name)
{
    Vector2* value = reinterpret_cast<Vector2*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    VectorField<Vector2>* field = row->SetValueWidget<VectorField<Vector2>>();
    field->SetValue(*value);

    field->Register("ValueChanged", this, &InspectorPanel::OnVector2Changed, &object, KeepProperty(property));
}

void InspectorPanel::AddVector3Property(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name)
{
    Vector3* value = reinterpret_cast<Vector3*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    VectorField<Vector3>* field = row->SetValueWidget<VectorField<Vector3>>();
    field->SetValue(*value);

    field->Register("ValueChanged", this, &InspectorPanel::OnVector3Changed, &object, KeepProperty(property));
}

void InspectorPanel::AddRotationProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name)
{
    Quaternion* value = reinterpret_cast<Quaternion*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    VectorField<Vector3>* field = row->SetValueWidget<VectorField<Vector3>>();
    field->SetValue(value->ToEulerDegrees());

    field->Register("ValueChanged", this, &InspectorPanel::OnRotationChanged, &object, KeepProperty(property));
}

void InspectorPanel::AddColorProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name)
{
    Color* value = reinterpret_cast<Color*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    ColorField* field = row->SetValueWidget<ColorField>();
    field->SetValue(*value);

    field->Register("ValueChanged", this, &InspectorPanel::OnColorChanged, &object, KeepProperty(property));
}

void InspectorPanel::AddVector4Property(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name)
{
    Vector4* value = reinterpret_cast<Vector4*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    VectorField<Vector4>* field = row->SetValueWidget<VectorField<Vector4>>();
    field->SetValue(*value);

    field->Register("ValueChanged", this, &InspectorPanel::OnVector4Changed, &object, KeepProperty(property));
}

void InspectorPanel::AddStringProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name)
{
    String* value = reinterpret_cast<String*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    StringField* field = row->SetValueWidget<StringField>();
    field->SetText(*value);

    field->Register("ValueChanged", this, &InspectorPanel::OnStringChanged, &object, KeepProperty(property), field);
}

void InspectorPanel::OnBoolChanged(bool value, Object* object, const Property* property)
{
    UpdateInspectedProperty(value, object, property);
}

void InspectorPanel::OnIntChanged(int32 value, Object* object, const Property* property)
{
    UpdateInspectedProperty(value, object, property);
}

void InspectorPanel::OnFloatChanged(float value, Object* object, const Property* property)
{
    UpdateInspectedProperty(value, object, property);
}

void InspectorPanel::OnVector2Changed(Vector2 value, Object* object, const Property* property)
{
    UpdateInspectedProperty(value, object, property);
}

void InspectorPanel::OnVector3Changed(Vector3 value, Object* object, const Property* property)
{
    UpdateInspectedProperty(value, object, property);
}

void InspectorPanel::OnRotationChanged(Vector3 value, Object* object, const Property* property)
{
    UpdateInspectedProperty(Quaternion::FromEulerDegrees(value), object, property);
}

void InspectorPanel::OnVector4Changed(Vector4 value, Object* object, const Property* property)
{
    UpdateInspectedProperty(value, object, property);
}

void InspectorPanel::OnColorChanged(Color value, Object* object, const Property* property)
{
    UpdateInspectedProperty(value, object, property);
}

void InspectorPanel::OnStringChanged(Object* object, const Property* property, StringField* field)
{
    if (field)
        UpdateInspectedProperty(field->GetText(), object, property);
}

void InspectorPanel::AddAssetProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name)
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

    field->Register("ValueChanged", this, &InspectorPanel::OnAssetChanged, &object, KeepProperty(property));
}

void InspectorPanel::OnAssetChanged(AssetHandle value, Object* object, const Property* property)
{
    if (!object || !property)
        return;

    const auto* current = reinterpret_cast<const AssetHandle*>(
        reinterpret_cast<const uint8*>(object) + property->offset);
    if (*current != value)
        UpdateInspectedProperty(value, object, property);
}

void InspectorPanel::OnAddComponentClicked(Button* button, Unit* unit)
{
    if (!button || !unit)
        return;

    PopupLayer& popupLayer = gEditor->GetEditorUI().GetPopupLayer();

    ComponentPickerPopup* popup = popupLayer.OpenPopup<ComponentPickerPopup>(button->GetRect(), *unit);

    const ObjectId unitId = unit->GetId();
    popup->Register("ComponentSelected", gEditor, &Editor::OnComponentPopupSelected, unitId);
}

} // namespace URay
