#pragma once

namespace URay
{

class AssetDatabase;
class Material;

namespace Render
{
class ResourceManager;
struct MaterialState;
struct ShaderReflection;
} // namespace Render

bool BuildMaterialState(
    const Material& material,
    const Render::ShaderReflection& reflection,
    AssetDatabase& assetDB,
    Render::ResourceManager& resourceManager,
    Render::MaterialState& outState);

} // namespace URay
