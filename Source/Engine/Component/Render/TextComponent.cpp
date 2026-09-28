#include "Engine/Component/Render/TextComponent.h"

#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/EngineAsset.h"
#include "Engine/Asset/Font/Font.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Unit.h"

#include "Render/Rendering/Object/Drawable/TextObject.h"

namespace URay
{

URAY_REGISTER_CLASS(TextComponent)
URAY_REGISTER_COMPONENT(TextComponent)

TextComponent::TextComponent()
{
    fontUUID = EngineAsset::BitmapFont;
}

void TextComponent::RegisterClass()
{
    StaticClass()->AddProperty(
        { .type = PropertyType::String,
          .name = "Text",
          .offset = offsetof(TextComponent, text),
          .size = sizeof(std::string) });
}

Render::RenderObject* TextComponent::CreateRenderObject()
{
    Unit* owner = GetOwner();
    if (!owner)
        return nullptr;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    TransformComponent* transform = owner->GetTransform();

    Render::TextObjectState state = {};
    state.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    state.font = assetDatabase.Find<Font>(fontUUID);
    state.text = text;

    renderObject = new Render::TextObject(state);
    return renderObject;
}

void TextComponent::UpdateRenderObject()
{
    Unit* owner = GetOwner();
    if (!owner)
        return;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    TransformComponent* transform = owner->GetTransform();

    Render::TextObjectState state = {};
    state.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    state.font = assetDatabase.Find<Font>(fontUUID);
    state.text = text;

    Render::TextObject* textObject = static_cast<Render::TextObject*>(renderObject);
    textObject->Update(state);
}

} // namespace URay
