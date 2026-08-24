#pragma once

#include "Render/Scene/Object/RenderObject.h"

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

class TextObject : public RenderObject
{
public:
    TextObject(const TextObjectState& state);
    ~TextObject();

public:
    void Update(const TextObjectState& state);
    void Submit(DrawCommandBuilder& builder) const override;

private:
    Font* font = nullptr;
    std::string text;
};

} // namespace Render

} // namespace URay
