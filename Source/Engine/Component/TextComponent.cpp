#include "Engine/Component/TextComponent.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Font/FontManager.h"
#include "Engine/Unit.h"

#include "Render/Scene/Object/TextObject.h"

namespace URay
{

URAY_REGISTER_CLASS(TextComponent)
URAY_REGISTER_COMPONENT(TextComponent)

TextComponent::TextComponent()
{
    font = gEngine->GetFontManager()->GetFont("Default");
}

void TextComponent::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty({ .type = PropertyType::String,
                                 .name = "Text",
                                 .offset = offsetof(TextComponent, text),
                                 .size = sizeof(std::string) });
}

Render::RenderObject* TextComponent::CreateRenderObject()
{
    Unit* owner = GetOwner();
    if (!owner)
        return nullptr;

    TransformComponent* transform = owner->GetTransform();

    Render::TextObjectState state = {};
    state.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    state.font = font;
    state.text = text;

    renderObject = new Render::TextObject(state);
    return renderObject;
}

} // namespace URay
