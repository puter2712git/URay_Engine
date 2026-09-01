#include "LineObject.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"

namespace URay::Render
{

LineObject::LineObject(const LineObjectState& state)
{
    lines = state.lines;
}

LineObject::~LineObject() = default;

void LineObject::Update(const LineObjectState& state)
{
    lines = state.lines;
}

void LineObject::Submit(DrawCommandBuilder& builder) const
{
    for (const Line& line : lines)
    {
        LineCommandContext context = {};
        context.start = line.start;
        context.end = line.end;
        context.color = line.color;

        builder.BuildLine(context);
    }
}

} // namespace URay::Render
