#include "RenderComponent.h"

#include "Engine/Scene/Unit.h"

#include "Render/Scene/Object/RenderObject.h"

namespace URay
{

URAY_REGISTER_CLASS(RenderComponent)

void RenderComponent::RegisterClass()
{
    Super::RegisterClass();
}

void RenderComponent::Update(float deltaTime)
{
    Super::Update(deltaTime);

    if (renderObject && renderObject->IsDirty())
    {
        UpdateRenderObject();
        renderObject->SetDirty(false);
    }
}

void RenderComponent::OnAttached()
{
    Super::OnAttached();

    Unit* owner = GetOwner();
    if (!owner)
        return;

    owner->RegisterTransformUpdateCallback([this]()
                                           { MarkDirty(); });
}

void RenderComponent::OnDetached()
{
}

void RenderComponent::MarkDirty()
{
    if (renderObject)
    {
        renderObject->SetDirty(true);
    }
}

} // namespace URay
