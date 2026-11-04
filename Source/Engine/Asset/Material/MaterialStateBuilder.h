#pragma once

namespace URay
{

class AssetDatabase;
class MaterialInterface;

namespace Render
{
class ResourceManager;
struct MaterialState;
struct ShaderReflection;
} // namespace Render

bool BuildMaterialState(
    const MaterialInterface& material,
    const Render::ShaderReflection& reflection,
    AssetDatabase& assetDB,
    Render::ResourceManager& resourceManager,
    Render::MaterialState& outState);

} // namespace URay
