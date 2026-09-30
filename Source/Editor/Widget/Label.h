#pragma once

#include "Editor/Widget/Widget.h"

#include "Render/Rendering/Batch/UIDrawContext.h"

#include <string>
#include <string_view>

namespace URay
{

class Label final : public Widget
{
public:
    Label(std::string_view text = {});
    ~Label() override;

public:
    const std::string_view& GetText() const { return text; }
    void SetText(std::string_view text) { this->text = text; }

    void SetTextStyle(const Render::TextStyle& style) { this->style = style; }

protected:
    void OnPaint(Render::UIDrawContext& context) override;

private:
    std::string text;
    Render::TextStyle style;
};

} // namespace URay
