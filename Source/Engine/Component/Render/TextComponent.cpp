#include "TextComponent.h"

#include "Engine/Font/FontManager.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Unit.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/DrawCommand/DrawCommandContext.h"

namespace URay
{

using namespace RHI;

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

void TextComponent::SubmitCommand(DrawCommandBuilder& builder)
{
    if (!font || text.empty())
        return;

    Unit* owner = GetOwner();
    TransformComponent* transform = owner ? owner->GetTransform() : nullptr;

    TextCommandContext context = {};
    context.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    context.text = text;
    context.font = font;

    builder.BuildFromText(context);
}

} // namespace URay
