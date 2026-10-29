#pragma once

#include <Core/Type/TypeAlias.h>
#include <Editor/Widget/Panel/Panel.h>

#include <functional>
#include <memory>
#include <unordered_map>

namespace URay
{

class ViewportController;

struct PanelContext
{
    ViewportController& viewportController;
};

struct PanelRegistration
{
    String title;
    std::function<std::unique_ptr<Panel>(PanelContext&)> constructor;
};

class PanelFactory
{
public:
    static bool Register(const String& typeName, PanelRegistration registration);
    static const PanelRegistration* Find(const String& typeName);
    static std::unique_ptr<Panel> Create(const String& typeName, PanelContext& context);

private:
    static std::unordered_map<String, PanelRegistration>& GetRegistry();
};

#define URAY_REGISTER_PANEL_WITH_CONTEXT(panel, title, ...)                           \
    namespace                                                                        \
    {                                                                                \
    [[maybe_unused]] const bool registered##panel = PanelFactory::Register(           \
        String{ panel::StaticType().name }, PanelRegistration{ title, __VA_ARGS__ });  \
    }

#define URAY_REGISTER_PANEL(panel, title)                                             \
    URAY_REGISTER_PANEL_WITH_CONTEXT(panel, title,                                    \
        [](PanelContext&) -> std::unique_ptr<Panel> { return std::make_unique<panel>(); })

} // namespace URay
