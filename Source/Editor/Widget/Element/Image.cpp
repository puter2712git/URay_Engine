#include "Image.h"

#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Engine.h>

#include <Render/RHI/Texture/TextureView.h>
#include <Render/RenderSystem.h>
#include <Render/Rendering/Batch/UIDrawContext.h>
#include <Render/ResourceManager.h>

namespace URay
{

Image::Image(AssetHandle textureHandle) : textureHandle(textureHandle) {}

void Image::OnPaint(Render::UIDrawContext& context)
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    Texture* textureAsset = assetDatabase.Find<Texture>(textureHandle);
    if (!textureAsset)
        return;

    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::ResourceManager& resourceManager = renderSystem.GetResourceManager();

    Render::Texture* texture = resourceManager.GetOrCreateTexture(textureAsset);
    if (!texture)
        return;

    Render::TextureView* view = resourceManager.GetOrCreateTextureView(texture, {});
    if (!view)
        return;

    Render::Sampler* sampler = resourceManager.GetOrCreateSampler({});
    if (!sampler)
        return;

    Render::UIImage imageSource = {};
    imageSource.view = view;
    imageSource.sampler = sampler;
    imageSource.uvMin = Vector2::Zero;
    imageSource.uvMax = Vector2::One;

    context.AddImage(rect, imageSource, color);
}

} // namespace URay
