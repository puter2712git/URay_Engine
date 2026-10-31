#pragma once

#include <Core/Math/Vector2.h>

#include <memory>

namespace URay::Render
{
class UIDrawContext;
}

namespace URay
{

class InputSystem;
class UIInputRouter;
class Widget;
class WindowWidget;
class Window;
struct WindowUIContext;

class UISystem
{
public:
    UISystem(Window& window, InputSystem& inputSystem);
    ~UISystem();

public:
    void Initialize();
    void Finalize();

    void SetRootWidget(std::unique_ptr<Widget> root);
    void ClearRootWidget();

    void Update(float deltaTime);
    void PrepareRender();
    void Paint();

    bool IsTextInputActive() const;

    bool TryHideWidget(Widget& widget);
    bool TryClearInputStateForWidget(Widget& widget);
    bool TryClearInputStateForSubtree(Widget& subtree);

    Widget* FindWidgetAt(const Vector2& position) const;

    Widget& GetRootWidget() const;
    Render::UIDrawContext& GetDrawContext() const;

private:
    Window& window;
    InputSystem& inputSystem;

    std::unique_ptr<WindowUIContext> mainContext = nullptr;
};

} // namespace URay
