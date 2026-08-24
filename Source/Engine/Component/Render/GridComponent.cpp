#include "GridComponent.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/DrawCommand/DrawCommandContext.h"
#include "Render/Scene/Object/LineObject.h"

namespace URay
{

using namespace RHI;

URAY_REGISTER_CLASS(GridComponent)

void GridComponent::RegisterClass()
{
    Super::RegisterClass();
}

RHI::RenderObject* GridComponent::CreateRenderObject()
{
    Line xCoordLine = {};
    xCoordLine.start = Vector3(-100.0f, 0.0f, 0.0f);
    xCoordLine.end = Vector3(100.0f, 0.0f, 0.0f);
    xCoordLine.color = Color::Red;

    Line yCoordLine = {};
    yCoordLine.start = Vector3(0.0f, -100.0f, 0.0f);
    yCoordLine.end = Vector3(0.0f, 100.0f, 0.0f);
    yCoordLine.color = Color::Green;

    Line zCoordLine = {};
    zCoordLine.start = Vector3(0.0f, 0.0f, -100.0f);
    zCoordLine.end = Vector3(0.0f, 0.0f, 100.0f);
    zCoordLine.color = Color::Blue;

    RHI::LineObjectState state = {};
    state.lines = { xCoordLine,
                    yCoordLine,
                    zCoordLine };

    renderObject = new RHI::LineObject(state);
    return renderObject;
}

} // namespace URay
