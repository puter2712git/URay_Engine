#include "TextObject.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"

namespace URay::Render
{

TextObject::TextObject(const TextObjectState& state)
    : state(state) {}

TextObject::~TextObject()
{
}

void TextObject::Update(const TextObjectState& state)
{
    this->state = state;
}

void TextObject::Submit(DrawCommandBuilder& builder) const
{
    TextCommandContext context = {};
    context.worldMatrix = state.worldMatrix;
    context.font = state.font;
    context.text = state.text;

    builder.BuildText(context);
}

} // namespace URay::Render
