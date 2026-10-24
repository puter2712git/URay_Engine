#pragma once

#include <Engine/UI/Widget/Widget.h>

namespace URay
{

class ScrollView final : public Widget
{
public:
    template <typename T, typename... Args>
    T* SetContent(Args&&... args)
    {
        T* result = AddChild<T>(std::forward<Args>(args)...);
        content = result;
        return result;
    }

    void Arrange(const Rect& rect) override;

    EventReply OnScroll(const ScrollEvent& event) override;

    void RequestScrollToBottom() { scrollToBottomRequested = true; }

protected:
    Vector2 OnMeasure() override;

    void OnPaintForeground(Render::UIDrawContext& context) override {}

    std::optional<Rect> GetClipRect() const override { return rect; }

private:
    Widget* content = nullptr;

    float scrollOffsetY = 0.0f;
    float maxScrollOffsetY = 0.0f;

    bool scrollToBottomRequested = false;
};

} // namespace URay
