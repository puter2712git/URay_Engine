#pragma once

#include "Render/Rendering/Object/Drawable/DrawableObject.h"

#include <string>

namespace URay
{

class Font;

namespace Render
{

struct TextObjectState
{
    Matrix worldMatrix = Matrix::Identity;
    Font* font = nullptr;
    std::string text;
};

class TextObject : public DrawableObject
{
public:
    TextObject(const TextObjectState& state);
    ~TextObject();

public:
    void Update(const TextObjectState& state);
    void Submit(DrawCommandBuilder& builder) const override;

private:
    TextObjectState state;
};

} // namespace Render

} // namespace URay
