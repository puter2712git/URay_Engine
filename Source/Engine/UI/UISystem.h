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
class Window;

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
    bool PrepareForDetach(Widget& subtree);

    Widget* FindWidgetAt(const Vector2& position) const;

    Widget& GetRootWidget() const { return *root; }
    Render::UIDrawContext& GetDrawContext() const { return *drawContext; }

private:
    Window& window;
    InputSystem& inputSystem;

    std::unique_ptr<Widget> root = nullptr;
    std::unique_ptr<UIInputRouter> inputRouter = nullptr;
    std::unique_ptr<Render::UIDrawContext> drawContext = nullptr;
};

} // namespace URay
