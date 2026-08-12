#include "GridComponent.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/DrawCommand/DrawCommandContext.h"

namespace URay
{

using namespace RHI;

URAY_REGISTER_CLASS(GridComponent)

void GridComponent::RegisterClass()
{
    Super::RegisterClass();
}

void GridComponent::SubmitCommand(DrawCommandBuilder& builder)
{
    LineCommandContext xCoord = {};
    xCoord.start = Vector3(-100.0f, 0.0f, 0.0f);
    xCoord.end = Vector3(100.0f, 0.0f, 0.0f);
    xCoord.color = Color::Red;

    LineCommandContext yCoord = {};
    yCoord.start = Vector3(0.0f, -100.0f, 0.0f);
    yCoord.end = Vector3(0.0f, 100.0f, 0.0f);
    yCoord.color = Color::Green;

    LineCommandContext zCoord = {};
    zCoord.start = Vector3(0.0f, 0.0f, -100.0f);
    zCoord.end = Vector3(0.0f, 0.0f, 100.0f);
    zCoord.color = Color::Blue;

    builder.BuildFromLine(xCoord);
    builder.BuildFromLine(yCoord);
    builder.BuildFromLine(zCoord);
}

} // namespace URay
