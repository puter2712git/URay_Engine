#pragma once

#include <Editor/Widget/Input/EventReply.h>

#include <Core/Input/InputEvent.h>
#include <Core/Math/Rect.h>
#include <Core/Type/RuntimeType.h>

#include <memory>
#include <optional>
#include <vector>

namespace URay::Render
{
class UIDrawContext;
}

namespace URay
{

using WidgetId = uint64;

inline constexpr WidgetId InvalidWidgetId = 0;

class Widget
{
    URAY_ROOT_TYPE(Widget)

    friend class UIInputRouter;

public:
    Widget();
    virtual ~Widget() = default;

public:
    WidgetId GetId() const { return id; }

    void Update(float deltaTime);
    void Paint(Render::UIDrawContext& context);

    Widget* GetParent() const { return parent; }

    void AddChild(std::unique_ptr<Widget> child);

    template <typename T, typename... Args>
        requires std::derived_from<T, Widget>
    T* AddChild(Args&&... args)
    {
        auto child = std::make_unique<T>(std::forward<Args>(args)...);
        T* result = child.get();

        AddChild(std::move(child));
        return result;
    }

    const std::vector<std::unique_ptr<Widget>>& GetChildren() const { return children; }

    void ClearChildren() { children.clear(); }

    void Measure();

    virtual void Arrange(const Rect& rect) { this->rect = rect; }

    const Vector2& GetDesiredSize() const { return desiredSize; }

    const std::optional<float>& GetPreferredWidth() const { return preferredWidth; }
    const std::optional<float>& GetPreferredHeight() const { return preferredHeight; }
    void SetPreferredWidth(float width) { preferredWidth = width; }
    void SetPreferredHeight(float height) { preferredHeight = height; }
    void SetPreferredSize(const Vector2& size)
    {
        preferredWidth = size.x;
        preferredHeight = size.y;
    }

    const Rect& GetRect() const { return rect; }
    virtual Rect GetContentRect() const { return rect; }

    bool IsHovered() const { return hovered; }
    bool IsFocused() const { return focused; }
    bool HasPointerCapture() const { return pointerCaptured; }
    virtual bool IsTextInputActive() const { return false; }

    bool HitTest(const Vector2& position) const;
    virtual bool CanHit() const { return true; }

    virtual EventReply OnPointerEnter() { return EventReply{}; }
    virtual EventReply OnPointerLeave() { return EventReply{}; }
    virtual EventReply OnPointerDown(const PointerEvent& event) { return EventReply{}; }
    virtual EventReply OnPointerMove(const PointerEvent& event) { return EventReply{}; }
    virtual EventReply OnPointerUp(const PointerEvent& event) { return EventReply{}; }
    virtual EventReply OnKeyDown(const KeyEvent& event) { return EventReply{}; }
    virtual EventReply OnKeyUp(const KeyEvent& event) { return EventReply{}; }
    virtual EventReply OnScroll(const ScrollEvent& event) { return EventReply{}; }
    virtual EventReply OnTextInput(const TextInputEvent& event) { return {}; }

protected:
    virtual Vector2 OnMeasure() { return Vector2::Zero; }

    virtual void OnUpdate(float) {}
    virtual void OnPaint(Render::UIDrawContext& context) {}
    virtual void OnPaintForeground(Render::UIDrawContext& context) {}

    virtual std::optional<Rect> GetClipRect() const { return std::nullopt; }

protected:
    Widget* parent = nullptr;
    std::vector<std::unique_ptr<Widget>> children;

    Rect rect = {};
    Vector2 desiredSize = Vector2::Zero;
    std::optional<float> preferredWidth = std::nullopt;
    std::optional<float> preferredHeight = std::nullopt;

    bool hovered = false;
    bool focused = false;
    bool pointerCaptured = false;

private:
    WidgetId id = InvalidWidgetId;
};

} // namespace URay
