#include "Engine/Component/TextComponent.h"

#include "Engine/Asset/AssetSystem.h"
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
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    font = assetSystem.FindFont("Default");
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

void TextComponent::Update(float deltaTime)
{
    Super::Update(deltaTime);

    if (renderObject && renderObject->IsDirty())
    {
        renderObject->SetDirty(false);

        Unit* owner = GetOwner();
        if (!owner)
            return;

        TransformComponent* transform = owner->GetTransform();

        Render::TextObjectState state = {};
        state.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
        state.font = font;
        state.text = text;

        renderObject->Update(state);
    }
}

void TextComponent::OnAttached()
{
    Unit* owner = GetOwner();

    owner->RegisterTransformUpdateCallback([this]()
                                           {
        if (renderObject)
        {
            renderObject->SetDirty(true);
    } });
}

void TextComponent::OnDetached()
{
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
