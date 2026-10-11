#pragma once

#include <Editor/Widget/Panel/Panel.h>

#include <string>

namespace URay
{

class SelectionSystem;
class VerticalLayout;
class ScrollView;
class Object;
struct Property;

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

    void AddProperty(Object& object, const Property& property, const std::string& name);

    void AddBoolProperty(Object& object, const Property& property, const std::string& name);
    void AddFloatProperty(Object& object, const Property& property, const std::string& name);
    void AddVector3Property(Object& object, const Property& property, const std::string& name);
    void AddColorProperty(Object& object, const Property& property, const std::string& name);
    void AddStringProperty(Object& object, const Property& property, const std::string& name);

private:
    SelectionSystem& selectionSystem;

    ScrollView* scrollView = nullptr;
    VerticalLayout* layout = nullptr;

    bool isDirty = true;
};

} // namespace URay
