#pragma once

#include <Editor/Widget/Style/EditorStyles.h>
#include <Editor/Widget/Widget.h>

#include <Engine/Asset/Asset.h>

#include <Core/Math/Color.h>
#include <Core/Ray/EventRay.h>

#include <string_view>

namespace URay
{

class Label;

enum class ButtonState
{
    Normal,
    Hovering,
    Pressed
};

class Button final : public Widget
{
    URAY_TYPE(Button, Widget)

public:
    explicit Button(std::string_view text);
    explicit Button(AssetHandle textureHandle);
    ~Button() override;

public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    ButtonState GetState() const;

    EventRay<>& GetOnClickRay() { return onClickRay; }

protected:
    Vector2 OnMeasure() override;

    void OnPaint(Render::UIDrawContext& context) override;

private:
    Widget* content = nullptr;

    Color normalColor = EditorStyle::ControlNormal;
    Color hoverColor = EditorStyle::ControlHovered;
    Color pressedColor = EditorStyle::ControlPressed;

    EventRay<> onClickRay;
};

} // namespace URay
