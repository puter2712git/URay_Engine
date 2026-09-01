#include "FogComponent.h"

#include "Render/Scene/Object/FogObject.h"

namespace URay
{

URAY_REGISTER_CLASS(FogComponent)
URAY_REGISTER_COMPONENT(FogComponent)

FogComponent::FogComponent() = default;

FogComponent::~FogComponent() = default;

void FogComponent::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty(
        { .type = PropertyType::Vector4,
          .name = "Fog Color",
          .offset = offsetof(FogComponent, fogColor),
          .size = sizeof(Color),
          .OnChangedCallback = [](Object* owner, const Property&)
          {
              FogComponent* fogComp = static_cast<FogComponent*>(owner);
              fogComp->UpdateRenderObject();
          } });

    StaticClass()->AddProperty(
        { .type = PropertyType::Float,
          .name = "Fog Start Distance",
          .offset = offsetof(FogComponent, fogStart),
          .size = sizeof(float),
          .OnChangedCallback = [](Object* owner, const Property&)
          {
              FogComponent* fogComp = static_cast<FogComponent*>(owner);
              fogComp->UpdateRenderObject();
          } });

    StaticClass()->AddProperty(
        { .type = PropertyType::Float,
          .name = "Fog End Distance",
          .offset = offsetof(FogComponent, fogEnd),
          .size = sizeof(float),
          .OnChangedCallback = [](Object* owner, const Property&)
          {
              FogComponent* fogComp = static_cast<FogComponent*>(owner);
              fogComp->UpdateRenderObject();
          } });

    StaticClass()->AddProperty(
        { .type = PropertyType::Float,
          .name = "Density",
          .offset = offsetof(FogComponent, density),
          .size = sizeof(float),
          .OnChangedCallback = [](Object* owner, const Property&)
          {
              FogComponent* fogComp = static_cast<FogComponent*>(owner);
              fogComp->UpdateRenderObject();
          } });
}

Render::RenderObject* FogComponent::CreateRenderObject()
{
    Render::FogObjectState state = {};
    state.fogColor = fogColor;
    state.fogStart = fogStart;
    state.fogEnd = fogEnd;
    state.density = density;

    renderObject = new Render::FogObject(state);
    return renderObject;
}

void FogComponent::UpdateRenderObject()
{
    Render::FogObjectState state = {};
    state.fogColor = fogColor;
    state.fogStart = fogStart;
    state.fogEnd = fogEnd;
    state.density = density;

    Render::FogObject* fogObject =
        static_cast<Render::FogObject*>(renderObject);
    if (fogObject)
    {
        fogObject->Update(state);
    }
}

} // namespace URay
