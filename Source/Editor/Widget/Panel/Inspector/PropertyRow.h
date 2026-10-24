#pragma once

#include <Engine/UI/Widget/Widget.h>

#include <string>

namespace URay
{

class Label;

class PropertyRow final : public Widget
{
public:
    explicit PropertyRow(const std::string& name);

public:
    void Arrange(const Rect& rect) override;

    template <typename T, typename... Args>
        requires std::derived_from<T, Widget>
    T* SetValueWidget(Args&&... args)
    {
        T* widget = AddChild<T>(std::forward<Args>(args)...);
        valueWidget = widget;

        return widget;
    }

protected:
    Vector2 OnMeasure() override;

private:
    Label* nameLabel = nullptr;
    Widget* valueWidget = nullptr;
};

} // namespace URay
