#pragma once

#include "Editor/Input/EventReply.h"

#include "Core/Input/InputEvent.h"
#include "Core/Math/Rect.h"

#include <memory>
#include <vector>

namespace URay
{

namespace Render
{
class DrawCommandBuilder;
}

class Widget
{
    friend class UIInputRouter;

public:
    virtual ~Widget() = default;

public:
    void Update(float deltaTime);
    void PrepareRender(Render::DrawCommandBuilder& builder);
    void Draw();

    void AddChild(std::unique_ptr<Widget> child);
    const std::vector<std::unique_ptr<Widget>>& GetChildren() const { return children; }

    virtual void Arrange(const Rect& rect) { this->rect = rect; }
    const Rect& GetRect() const { return rect; }

    bool IsHovered() const { return hovered; }
    bool IsFocused() const { return focused; }
    bool HasPointerCapture() const { return pointerCaptured; }

    bool HitTest(const Vector2& position) const;

    virtual EventReply OnPointerEnter() { return EventReply{}; }
    virtual EventReply OnPointerLeave() { return EventReply{}; }
    virtual EventReply OnPointerDown(const PointerEvent& event) { return EventReply{}; }
    virtual EventReply OnPointerMove(const PointerEvent& event) { return EventReply{}; }
    virtual EventReply OnPointerUp(const PointerEvent& event) { return EventReply{}; }
    virtual EventReply OnKeyDown(const KeyEvent& event) { return EventReply{}; }
    virtual EventReply OnKeyUp(const KeyEvent& event) { return EventReply{}; }
    virtual EventReply OnScroll(const ScrollEvent& event) { return EventReply{}; }

protected:
    void ApplyRect() const;

    virtual void OnUpdate(float) {}
    virtual void OnPrepareRender(Render::DrawCommandBuilder& builder) {}
    virtual void OnDraw() {}

protected:
    std::vector<std::unique_ptr<Widget>> children;

    Rect rect = {};

    bool hovered = false;
    bool focused = false;
    bool pointerCaptured = false;
};

} // namespace URay
