#include "InspectorPanel.h"

#include <Core/Math/Color.h>
#include <Core/Math/Quaternion.h>
#include <Core/Math/Vector2.h>
#include <Core/Math/Vector3.h>
#include <Core/Math/Vector4.h>
#include <Core/Type/TypeAlias.h>
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
#include <Editor/Widget/Panel/PanelFactory.h>
#include <Editor/Widget/Panel/ScrollView.h>
#include <Editor/Widget/Popup/ComponentPickerPopup.h>
#include <Editor/Widget/Style/EditorStyles.h>
#include <Engine/Asset/Asset.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Material/Material.h>
#include <Engine/Asset/Mesh/Mesh.h>
#include <Engine/Asset/Mesh/SkinnedMesh.h>
#include <Engine/Asset/Script/Script.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Engine/Component/Component.h>
#include <Engine/Component/ScriptComponent.h>
#include <Engine/Engine.h>
#include <Engine/Object/Object.h>
#include <Engine/Object/Property/Property.h>
#include <Engine/Scene/SceneSystem.h>
#include <Engine/Scene/Unit.h>
#include <Engine/Script/ScriptSystem.h>
#include <Engine/UI/Widget/Element/Button.h>
#include <Engine/UI/Widget/Element/Checkbox.h>
#include <Engine/UI/Widget/Layout/VerticalLayout.h>

#include <algorithm>
#include <utility>

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

URAY_REGISTER_PANEL(InspectorPanel, "Inspector")

InspectorPanel::InspectorPanel() : inspectSystem(gEditor->GetInspectSystem())
{
    SetBackgroundColor(EditorStyle::Surface);
    SetPadding(Padding(5.0f, 5.0f));

    scrollView = AddChild<ScrollView>();

    layout = scrollView->SetContent<VerticalLayout>();
    layout->SetGap(4.0f);
    layout->SetCrossAxisAlignment(CrossAxisAlignment::Stretch);

    inspectSystem.Register("TargetChanged", this, &InspectorPanel::OnTargetChanged);
    inspectSystem.Register("TargetStructureChanged", this, &InspectorPanel::OnTargetStructureChanged);
    inspectSystem.Register("TargetValuesChanged", this, &InspectorPanel::OnTargetValuesChanged);
}

InspectorPanel::~InspectorPanel()
{
    inspectSystem.UnregisterAll(this);
    propertySynchronizers.clear();
    layout->ClearChildren();
    propertyCopies.clear();
}

void InspectorPanel::OnTargetChanged(const InspectTarget*, const InspectTarget*)
{
    isDirty = true;
}

void InspectorPanel::OnTargetStructureChanged(const InspectTarget*)
{
    isDirty = true;
}

void InspectorPanel::OnTargetValuesChanged(const InspectTarget*)
{
    valuesDirty = true;
}

void InspectorPanel::Arrange(const Rect& rect)
{
    Panel::Arrange(rect);
    scrollView->Arrange(GetContentRect());
}

bool InspectorPanel::AcceptScriptAsset(AssetHandle handle)
{
    const auto* target = std::get_if<UnitInspectTarget>(&inspectSystem.GetTarget().data);
    Script* script = gEngine->GetAssetSystem().GetDatabase().Find<Script>(handle);
    if (!target || !target->unit || !script)
        return false;

    const Vector<String>& scriptTypes = gEngine->GetScriptSystem().GetScriptTypes();

    const String& typeName = script->GetTypeName();

    if (std::find(scriptTypes.begin(), scriptTypes.end(), typeName) == scriptTypes.end())
        return false;

    auto component = MakeUnique<ScriptComponent>();
    component->SetScriptTypeName(script->GetTypeName());
    target->unit->AddComponent(std::move(component));
    return true;
}

EventReply InspectorPanel::OnPointerDown(const PointerEvent& event)
{
    return EventReply{
        .requestFocus = true,
    };
}

void InspectorPanel::OnUpdate(float)
{
    if (isDirty)
    {
        isDirty = false;
        valuesDirty = false;
        RebuildProperties();
        return;
    }

    if (valuesDirty)
    {
        valuesDirty = false;

        for (const auto& synchronize : propertySynchronizers)
        {
            if (!synchronize())
                valuesDirty = true;
        }
    }
}

void InspectorPanel::RebuildProperties()
{
    propertySynchronizers.clear();
    layout->ClearChildren();
    propertyCopies.clear();
    inspectedScriptFields.clear();

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
        ScriptComponent* scriptComponent = Cast<ScriptComponent>(comp.get());

        String sectionName = comp->GetClass()->GetType().name;

        if (scriptComponent)
        {
            const String& typeName = scriptComponent->GetScriptTypeName();
            const Vector<String>& scriptTypes = gEngine->GetScriptSystem().GetScriptTypes();

            const bool isAvailable = std::find(scriptTypes.begin(), scriptTypes.end(), typeName) != scriptTypes.end();

            sectionName = typeName.empty() ? "Script" : typeName;

            if (!isAvailable)
                sectionName += " (Unavailable)";
        }

        ComponentSection* section = layout->AddChild<ComponentSection>(sectionName);

        for (const Property& property : comp->GetClass()->GetAllProperties())
        {
            if (scriptComponent && property.name == "Script Type")
                continue;

            AddProperty(*section->GetContent(), *comp, property, property.name);
        }
        if (scriptComponent)
        {
            AddScriptFields(*section->GetContent(), *scriptComponent);
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

void InspectorPanel::AddScriptFields(VerticalLayout& targetLayout, ScriptComponent& component)
{
    ScriptSystem& scriptSystem = gEngine->GetScriptSystem();

    Vector<ScriptFieldInfo> fields;

    if (!scriptSystem.GetScriptFields(component.GetScriptTypeName().c_str(), fields))
        return;

    for (const ScriptFieldInfo& fieldInfo : fields)
    {
        const ScriptFieldOverride* fieldOverride = component.GetFieldOverride(fieldInfo);
        const std::optional<Variant>& selectedValue = fieldOverride ? fieldOverride->value : fieldInfo.defaultValue;
        const int32 fieldIndex = static_cast<int32>(inspectedScriptFields.size());

        inspectedScriptFields.push_back(fieldInfo);

        switch (fieldInfo.type)
        {
        case ScriptFieldType::Bool:
        {
            const bool* value = selectedValue ? selectedValue->TryGet<bool>() : nullptr;
            if (!value)
                break;

            PropertyRow* row = targetLayout.AddChild<PropertyRow>(fieldInfo.name);

            Checkbox* checkbox = row->SetValueWidget<Checkbox>();
            ConfigureEditorCheckbox(*checkbox);
            checkbox->SetChecked(*value);

            checkbox->Register("ValueChanged", this, &InspectorPanel::OnScriptBoolChanged, &component, fieldIndex);
            break;
        }

        case ScriptFieldType::Int:
        {
            const int32* value = selectedValue ? selectedValue->TryGet<int32>() : nullptr;
            if (!value)
                break;

            PropertyRow* row = targetLayout.AddChild<PropertyRow>(fieldInfo.name);

            NumberField<int32>* numberField = row->SetValueWidget<NumberField<int32>>();
            numberField->SetValue(*value);

            numberField->Register("ValueChanged", this, &InspectorPanel::OnScriptIntChanged, &component, fieldIndex);
            break;
        }

        case ScriptFieldType::Float:
        {
            const float* value = selectedValue ? selectedValue->TryGet<float>() : nullptr;
            if (!value)
                break;

            PropertyRow* row = targetLayout.AddChild<PropertyRow>(fieldInfo.name);

            NumberField<float>* numberField = row->SetValueWidget<NumberField<float>>();
            numberField->SetValue(*value);

            numberField->Register("ValueChanged", this, &InspectorPanel::OnScriptFloatChanged, &component, fieldIndex);
            break;
        }

        case ScriptFieldType::Vector3:
        {
            const Vector3* value = selectedValue ? selectedValue->TryGet<Vector3>() : nullptr;
            if (!value)
                break;

            PropertyRow* row = targetLayout.AddChild<PropertyRow>(fieldInfo.name);

            VectorField<Vector3>* vectorField = row->SetValueWidget<VectorField<Vector3>>();
            vectorField->SetValue(*value);

            vectorField->Register("ValueChanged", this, &InspectorPanel::OnScriptVector3Changed, &component, fieldIndex);
            break;
        }

        case ScriptFieldType::String:
        {
            const String* value = selectedValue ? selectedValue->TryGet<String>() : nullptr;
            if (selectedValue && !value)
                break;

            PropertyRow* row = targetLayout.AddChild<PropertyRow>(fieldInfo.name);

            StringField* stringField = row->SetValueWidget<StringField>();
            stringField->SetText(value ? *value : String{});

            stringField->Register("ValueChanged", this, &InspectorPanel::OnScriptStringChanged, &component, fieldIndex, stringField);
            break;
        }

        default:
            break;
        }
    }
}

void InspectorPanel::SetScriptFieldValue(ScriptComponent* component, int32 fieldIndex, std::optional<Variant> value)
{
    if (!component || fieldIndex < 0 || static_cast<size_t>(fieldIndex) >= inspectedScriptFields.size())
        return;

    const ScriptFieldInfo& fieldInfo = inspectedScriptFields[static_cast<size_t>(fieldIndex)];

    component->SetFieldOverride(fieldInfo, std::move(value));
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
    propertyCopies.push_back(MakeUnique<Property>(property));
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

    propertySynchronizers.emplace_back([checkbox, value]() {
        checkbox->SetChecked(*value);
        return true;
    });
}

void InspectorPanel::AddFloatProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name)
{
    float* value = reinterpret_cast<float*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    NumberField<float>* field = row->SetValueWidget<NumberField<float>>();
    field->SetValue(*value);

    field->Register("ValueChanged", this, &InspectorPanel::OnFloatChanged, &object, KeepProperty(property));

    propertySynchronizers.emplace_back([field, value]() {
        if (field->IsInteracting())
            return false;

        field->SetValue(*value);
        return true;
    });
}

void InspectorPanel::AddIntProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name)
{
    int32* value = reinterpret_cast<int32*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    NumberField<int32>* field = row->SetValueWidget<NumberField<int32>>();
    field->SetValue(*value);

    field->Register("ValueChanged", this, &InspectorPanel::OnIntChanged, &object, KeepProperty(property));

    propertySynchronizers.emplace_back([field, value]() {
        if (field->IsInteracting())
            return false;

        field->SetValue(*value);
        return true;
    });
}

void InspectorPanel::AddVector2Property(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name)
{
    Vector2* value = reinterpret_cast<Vector2*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    VectorField<Vector2>* field = row->SetValueWidget<VectorField<Vector2>>();
    field->SetValue(*value);

    field->Register("ValueChanged", this, &InspectorPanel::OnVector2Changed, &object, KeepProperty(property));

    propertySynchronizers.emplace_back([field, value]() {
        return field->SynchronizeValue(*value);
    });
}

void InspectorPanel::AddVector3Property(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name)
{
    Vector3* value = reinterpret_cast<Vector3*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    VectorField<Vector3>* field = row->SetValueWidget<VectorField<Vector3>>();
    field->SetValue(*value);

    field->Register("ValueChanged", this, &InspectorPanel::OnVector3Changed, &object, KeepProperty(property));

    propertySynchronizers.emplace_back([field, value]() {
        return field->SynchronizeValue(*value);
    });
}

void InspectorPanel::AddRotationProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name)
{
    Quaternion* value = reinterpret_cast<Quaternion*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    VectorField<Vector3>* field = row->SetValueWidget<VectorField<Vector3>>();
    field->SetValue(value->ToEulerDegrees());

    field->Register("ValueChanged", this, &InspectorPanel::OnRotationChanged, &object, KeepProperty(property));

    propertySynchronizers.emplace_back([field, value]() {
        return field->SynchronizeValue(value->ToEulerDegrees());
    });
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

    propertySynchronizers.emplace_back([field, value]() {
        return field->SynchronizeValue(*value);
    });
}

void InspectorPanel::AddStringProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name)
{
    String* value = reinterpret_cast<String*>(reinterpret_cast<uint8*>(&object) + property.offset);

    PropertyRow* row = targetLayout.AddChild<PropertyRow>(name);

    StringField* field = row->SetValueWidget<StringField>();
    field->SetText(*value);

    field->Register("ValueChanged", this, &InspectorPanel::OnStringChanged, &object, KeepProperty(property), field);

    propertySynchronizers.emplace_back([field, value]() {
        if (field->IsEditing())
            return false;

        field->SetText(*value);
        return true;
    });
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

    propertySynchronizers.emplace_back([field, value]() {
        field->SetValue(*value);
        return true;
    });
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

void InspectorPanel::OnAssetChanged(AssetHandle value, Object* object, const Property* property)
{
    if (!object || !property)
        return;

    const auto* current = reinterpret_cast<const AssetHandle*>(
        reinterpret_cast<const uint8*>(object) + property->offset);
    if (*current != value)
        UpdateInspectedProperty(value, object, property);
}

void InspectorPanel::OnScriptBoolChanged(bool value, ScriptComponent* component, int32 fieldIndex)
{
    SetScriptFieldValue(component, fieldIndex, std::optional<Variant>(std::in_place, value));
}

void InspectorPanel::OnScriptIntChanged(int32 value, ScriptComponent* component, int32 fieldIndex)
{
    SetScriptFieldValue(component, fieldIndex, std::optional<Variant>(std::in_place, value));
}

void InspectorPanel::OnScriptFloatChanged(float value, ScriptComponent* component, int32 fieldIndex)
{
    SetScriptFieldValue(component, fieldIndex, std::optional<Variant>(std::in_place, value));
}

void InspectorPanel::OnScriptVector3Changed(Vector3 value, ScriptComponent* component, int32 fieldIndex)
{
    SetScriptFieldValue(component, fieldIndex, std::optional<Variant>(std::in_place, value));
}

void InspectorPanel::OnScriptStringChanged(ScriptComponent* component, int32 fieldIndex, StringField* field)
{
    if (!field)
        return;

    SetScriptFieldValue(component, fieldIndex, std::optional<Variant>(std::in_place, field->GetText()));
}

} // namespace URay
