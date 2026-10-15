#pragma once

#include <memory>

namespace URay::Render
{
class UIDrawContext;
}

namespace URay
{

class Widget;
class ViewportController;
class PopupLayer;
class DialogLayer;
class UIInputRouter;

class WidgetSystem
{
public:
    explicit WidgetSystem(ViewportController& viewportController);
    ~WidgetSystem();

public:
    bool Initialize();
    void Finalize();

    void Update(float deltaTime);
    void PrepareRender();
    void Paint();

    Widget& GetRootWidget() const { return *root; }
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
    ViewportController& viewportController;
};

} // namespace URay
