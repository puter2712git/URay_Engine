#pragma once

#include <memory>

namespace URay::Render
{
class UIDrawContext;
}

namespace URay
{

class Widget;
class ViewportPanel;
class WidgetDrawer;
class UIInputRouter;

class WidgetSystem
{
public:
    WidgetSystem();
    ~WidgetSystem();

public:
    bool Initialize();
    void Finalize();

    void Update(float deltaTime);
    void PrepareRender();
    void Paint();

    Widget& GetRootWidget() const { return *root; }
    ViewportPanel& GetViewport() const { return *viewport; }

    Render::UIDrawContext& GetDrawContext() const { return *drawContext; }

private:
    void CreateDefaultWidgets();

private:
    std::unique_ptr<Widget> root = nullptr;

    std::unique_ptr<WidgetDrawer> drawer = nullptr;
    std::unique_ptr<UIInputRouter> inputRouter = nullptr;

    std::unique_ptr<Render::UIDrawContext> drawContext = nullptr;

    ViewportPanel* viewport = nullptr;
};

} // namespace URay
