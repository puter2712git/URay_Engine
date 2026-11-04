#include "MaterialInterface.h"

#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Material/MaterialStateBuilder.h>
#include <Engine/Asset/Shader/Shader.h>
#include <Engine/Engine.h>
#include <Render/RenderSystem.h>
#include <Render/Rendering/Material/Material.h>
#include <Render/Rendering/RenderInfo.h>
#include <Render/ResourceManager.h>
#include <Render/Shader/Shader.h>

namespace URay
{

URAY_REGISTER_CLASS(MaterialInterface)

void MaterialInterface::RegisterClass()
{
}

bool MaterialInterface::PrepareRenderMaterial(uint32 frameIndex)
{
    if (!IsInitialized())
        return false;

    if (frameIndex >= Render::MAX_FRAMES_IN_FLIGHT)
        return false;

    Shader* shader = GetShader();
    if (!shader)
        return false;

    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::ResourceManager& resourceManager = renderSystem.GetResourceManager();

    Render::Shader* renderShader = resourceManager.GetOrCreateShader(shader, {});
    if (!renderShader)
        return false;

    const Render::DescriptorSetLayoutDesc* layoutDesc = renderShader->GetLayoutDescription(1);
    if (!layoutDesc)
        return true;

    Render::Material* renderMaterial = resourceManager.GetOrCreateMaterial(this);
    if (!renderMaterial)
        return false;

    if (!renderMaterial->Initialize(
            renderSystem.GetDevice(),
            resourceManager,
            *layoutDesc,
            renderShader->GetMergedReflection()))
    {
        return false;
    }

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDB = assetSystem.GetDatabase();

    Render::MaterialState materialState = {};
    if (!BuildMaterialState(
            *this,
            renderShader->GetMergedReflection(),
            assetDB,
            resourceManager,
            materialState))
    {
        return false;
    }

    renderMaterial->Update(materialState);

    return renderMaterial->Prepare(frameIndex);
}

} // namespace URay
