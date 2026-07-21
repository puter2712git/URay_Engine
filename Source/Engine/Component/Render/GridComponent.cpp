#include "GridComponent.h"

#include "Render/DrawCommand/DrawCommandContext.h"

namespace URay
{

void GridComponent::SubmitCommand(DrawCommandBuilder& builder)
{
    LineCommandContext context = {};
    context.start = Vector3(-100.0f, 0.0f, 0.0f);
    context.end = Vector3(100.0f, 0.0f, 0.0f);
    context.color = Color::Red;
}

} // namespace URay
