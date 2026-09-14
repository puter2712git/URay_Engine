#pragma once

#include <memory>

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

    Widget& GetRootWidget() const { return *root; }
    ViewportWidget& GetViewport() const { return *viewport; }

private:
    std::unique_ptr<Widget> mainMenuBar = nullptr;
    std::unique_ptr<Widget> root = nullptr;

    std::unique_ptr<WidgetDrawer> drawer = nullptr;
    std::unique_ptr<UIInputRouter> inputRouter = nullptr;

    ViewportWidget* viewport = nullptr;
};

} // namespace URay
