#include "TextComponent.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Asset/Font/FontManager.h"
#include "Engine/Unit.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/DrawCommand/DrawCommandContext.h"

namespace URay
{

URAY_REGISTER_CLASS(TextComponent)
URAY_REGISTER_COMPONENT(TextComponent)

TextComponent::TextComponent()
{
    font = gEngine->GetFontManager()->GetFontAsset("Default");
}

void TextComponent::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty({ PropertyType::String, "Text",
                                 offsetof(TextComponent, text), sizeof(std::string) });
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

bool TextComponent::Pick(const Vector3& origin, const Vector3& direction, float& outDist) const
{
    return false;
}

} // namespace URay
