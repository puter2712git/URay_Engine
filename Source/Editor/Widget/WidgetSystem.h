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
class PopupLayer;
class DialogLayer;
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
    PopupLayer& GetPopupLayer() const;
    DialogLayer& GetDialogLayer() const;
    bool IsTextInputActive() const;

    Render::UIDrawContext& GetDrawContext() const { return *drawContext; }

private:
    void CreateDefaultWidgets();

private:
    std::unique_ptr<Widget> root = nullptr;

    std::unique_ptr<UIInputRouter> inputRouter = nullptr;

    std::unique_ptr<Render::UIDrawContext> drawContext = nullptr;

    ViewportPanel* viewport = nullptr;
};

} // namespace URay
