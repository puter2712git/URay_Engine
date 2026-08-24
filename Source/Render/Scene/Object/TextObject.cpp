#include "TextObject.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"

namespace URay::RHI
{

TextObject::TextObject(const TextObjectState& state)
{
    worldMatrix = state.worldMatrix;
    font = state.font;
    text = state.text;
}

TextObject::~TextObject()
{
}

void TextObject::Update(const TextObjectState& state)
{
    worldMatrix = state.worldMatrix;
    font = state.font;
    text = state.text;
}

void TextObject::Submit(DrawCommandBuilder& builder) const
{
    TextCommandContext context = {};
    context.worldMatrix = worldMatrix;
    context.font = font;
    context.text = text;

    builder.BuildText(context);
}

} // namespace URay::RHI
