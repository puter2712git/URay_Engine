#pragma once
#include <Core/Type/TypeAlias.h>

#include <Editor/Widget/Panel/Panel.h>


namespace URay
{

class VerticalLayout;
class ScrollView;
class Object;
class Unit;
class InspectSystem;
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
    void RebuildProperties();

    void RebuildUnitProperties(Unit& unit);
    void RebuildAssetProperties(AssetHandle handle);

    void AddProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const String& name);

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

private:
    InspectSystem& inspectSystem;

    ScrollView* scrollView = nullptr;
    VerticalLayout* layout = nullptr;

    bool isDirty = true;
};

} // namespace URay
