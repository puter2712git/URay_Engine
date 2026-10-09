#pragma once

#include <Editor/Widget/Style/EditorStyles.h>
#include <Editor/Widget/Widget.h>

#include <Core/Math/Color.h>
#include <Core/Math/Padding.h>

#include <string>

namespace URay
{

class PanelHeader;

class Panel : public Widget
{
    URAY_TYPE(Panel, Widget)

public:
    Panel();
    Panel(const std::string& name);
    ~Panel() override;

public:
    void Arrange(const Rect& rect) override;

    Rect GetContentRect() const override;

    const Color& GetBackgroundColor() const { return backgroundColor; }
    void SetBackgroundColor(const Color& color) { backgroundColor = color; }

    float GetOutlineWidth() const { return outlineWidth; }
    void SetOutlineWidth(float width) { outlineWidth = width; }

    const Color& GetOutlineColor() const { return outlineColor; }
    void SetOutlineColor(const Color& color) { outlineColor = color; }

    const Padding& GetPadding() const { return padding; }
    void SetPadding(const Padding& padding) { this->padding = padding; }

protected:
    virtual void OnPaint(Render::UIDrawContext& context) override;
    virtual void OnPaintForeground(Render::UIDrawContext& context) override;

protected:
    PanelHeader* header = nullptr;

    std::string name;

    Color backgroundColor = EditorStyle::Surface;

    float outlineWidth = 0.0f;
    Color outlineColor;

    Padding padding = {};
};

} // namespace URay
