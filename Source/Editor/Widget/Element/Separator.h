#pragma once

#include <Editor/Widget/Style/EditorStyles.h>
#include <Engine/UI/Widget.h>

#include <Core/Math/Color.h>
#include <Core/Math/Padding.h>

namespace URay
{

enum class SeparatorOrientation
{
    Horizontal,
    Vertical
};

class Separator final : public Widget
{
public:
    explicit Separator(SeparatorOrientation orientation = SeparatorOrientation::Horizontal);

public:
    bool CanHit() const override { return false; }

protected:
    Vector2 OnMeasure() override;

    void OnPaint(Render::UIDrawContext& context) override;

private:
    SeparatorOrientation orientation = SeparatorOrientation::Horizontal;
    Color color = EditorStyle::Separator;
    float thickness = 1.0f;
    Padding padding = Padding(0.0f, 4.0f);
};

} // namespace URay
