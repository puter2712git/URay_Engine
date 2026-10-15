#pragma once

#include <Editor/Widget/Panel/Panel.h>

#include <string>

namespace URay
{

class VerticalLayout;
class ScrollView;
class Object;
class Unit;
class ScriptComponent;
class InspectSystem;
struct Property;
struct ScriptFieldValue;

class InspectorPanel final : public Panel
{
    URAY_TYPE(InspectorPanel, Panel)

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

    void AddProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name);
    void AddScriptField(VerticalLayout& targetLayout, ScriptComponent& component, const ScriptFieldValue& field);

    void AddBoolProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name);
    void AddIntProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name);
    void AddFloatProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name);
    void AddVector2Property(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name);
    void AddVector3Property(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name);
    void AddVector4Property(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name);
    void AddColorProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name);
    void AddStringProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name);
    void AddAssetProperty(VerticalLayout& targetLayout, Object& object, const Property& property, const std::string& name);
    void AddScriptProperty(VerticalLayout& targetLayout, ScriptComponent& component);

private:
    InspectSystem& inspectSystem;

    ScrollView* scrollView = nullptr;
    VerticalLayout* layout = nullptr;

    bool isDirty = true;
};

} // namespace URay
