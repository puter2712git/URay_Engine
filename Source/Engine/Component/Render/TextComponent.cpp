#include "Engine/Component/Render/TextComponent.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Unit.h"

#include "Render/Scene/Object/Drawable/TextObject.h"

namespace URay
{

URAY_REGISTER_CLASS(TextComponent)
URAY_REGISTER_COMPONENT(TextComponent)

TextComponent::TextComponent()
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    font = assetSystem.GetDefaultAssets().font;
}

void TextComponent::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty({ .type = PropertyType::String,
                                 .name = "Text",
                                 .offset = offsetof(TextComponent, text),
                                 .size = sizeof(std::string),
                                 .OnChangedCallback = [](Object* owner, const Property&)
                                 {
                                     TextComponent* textComp = static_cast<TextComponent*>(owner);
                                     if (textComp->renderObject)
                                     {
                                         textComp->renderObject->SetDirty(true);
                                     }
                                 } });
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

void TextComponent::UpdateRenderObject()
{
    Unit* owner = GetOwner();
    if (!owner)
        return;

    TransformComponent* transform = owner->GetTransform();

    Render::TextObjectState state = {};
    state.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    state.font = font;
    state.text = text;

    Render::TextObject* textObject = static_cast<Render::TextObject*>(renderObject);
    textObject->Update(state);
}

} // namespace URay
