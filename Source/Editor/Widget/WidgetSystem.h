#pragma once

#include <memory>

namespace URay::Render
{
class UIDrawContext;
}

namespace URay
{

class Widget;
class ViewportWidget;
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
    ViewportWidget& GetViewport() const { return *viewport; }

    Render::UIDrawContext& GetDrawContext() const { return *drawContext; }

private:
    void CreateDefaultWidgets();

private:
    std::unique_ptr<Widget> mainMenuBar = nullptr;
    std::unique_ptr<Widget> root = nullptr;

    std::unique_ptr<WidgetDrawer> drawer = nullptr;
    std::unique_ptr<UIInputRouter> inputRouter = nullptr;

    std::unique_ptr<Render::UIDrawContext> drawContext = nullptr;

    ViewportWidget* viewport = nullptr;
};

} // namespace URay
