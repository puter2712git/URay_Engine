#pragma once
#include <Core/Type/TypeAlias.h>

#include <Editor/Widget/Panel/Panel.h>

#include <memory>
#include <vector>

namespace URay
{

class VerticalLayout;
class ScrollView;
class Button;
class Object;
class Unit;
class InspectSystem;
class StringField;
struct Color;
struct Vector3;
struct Vector4;
struct InspectTarget;
struct Property;

class InspectorPanel final : public Panel
{
    URAY_CLASS(InspectorPanel, Panel)

public:
    InspectorPanel();
    ~InspectorPanel() override;

public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;

protected:
    void OnUpdate(float deltaTime) override;

private:
    void OnTargetChanged(const InspectTarget* previous, const InspectTarget* current);
    void OnTargetContentChanged(const InspectTarget* target);
    void RebuildProperties();

    void RebuildUnitProperties(Unit& unit);
    void RebuildAssetProperties(AssetHandle handle);

    void AddProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name);
    const Property* KeepProperty(const Property& property);

    void AddBoolProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name);
    void AddIntProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name);
    void AddFloatProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name);
    void AddVector2Property(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name);
    void AddVector3Property(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name);
    void AddRotationProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name);
    void AddVector4Property(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name);
    void AddColorProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name);
    void AddStringProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name);
    void AddAssetProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name);

    void OnAddComponentClicked(Button* button, Unit* unit);
    void OnBoolChanged(bool value, Object* object, const Property* property);
    void OnIntChanged(int32 value, Object* object, const Property* property);
    void OnFloatChanged(float value, Object* object, const Property* property);
    void OnVector2Changed(Vector2 value, Object* object, const Property* property);
    void OnVector3Changed(Vector3 value, Object* object, const Property* property);
    void OnRotationChanged(Vector3 value, Object* object, const Property* property);
    void OnVector4Changed(Vector4 value, Object* object, const Property* property);
    void OnColorChanged(Color value, Object* object, const Property* property);
    void OnStringChanged(Object* object, const Property* property, StringField* field);
    void OnAssetChanged(AssetHandle value, Object* object, const Property* property);

private:
    InspectSystem& inspectSystem;

    ScrollView* scrollView = nullptr;
    VerticalLayout* layout = nullptr;
    std::vector<std::unique_ptr<Property>> propertyCopies;

    bool isDirty = true;
};

} // namespace URay
