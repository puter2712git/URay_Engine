#include "MaterialInstance.h"

#include <Engine/Asset/Material/Material.h>

#include <utility>

namespace URay
{

URAY_REGISTER_CLASS(MaterialInstance)

void MaterialInstance::RegisterClass()
{
}

MaterialInstance::MaterialInstance(const Material* parent) : parent(parent)
{
}

const Material* MaterialInstance::GetParent() const
{
    return parent;
}

Shader* MaterialInstance::GetShader() const
{
    return parent ? parent->GetShader() : nullptr;
}

bool MaterialInstance::TryGetParameter(const String& name, MaterialParameter& outParameter) const
{
    if (!parent)
        return false;

    MaterialParameter parameter = {};
    if (!parent->TryGetParameter(name, parameter))
        return false;

    const auto it = overrides.find(name);
    if (it != overrides.end())
    {
        if (it->second.index() != parameter.value.index())
            return false;

        parameter.value = it->second;
    }

    outParameter = std::move(parameter);
    return true;
}

bool MaterialInstance::SetFloat(const String& name, float value)
{
    return SetOverride(name, MaterialParameterType::Float, value);
}

bool MaterialInstance::SetFloat2(const String& name, const Vector2& value)
{
    return SetOverride(name, MaterialParameterType::Float2, value);
}

bool MaterialInstance::SetFloat3(const String& name, const Vector3& value)
{
    return SetOverride(name, MaterialParameterType::Float3, value);
}

bool MaterialInstance::SetFloat4(const String& name, const Color& value)
{
    return SetOverride(name, MaterialParameterType::Float4, value);
}

bool MaterialInstance::SetTexture(const String& name, const AssetHandle& textureHandle)
{
    return SetOverride(name, MaterialParameterType::Texture2D, textureHandle);
}

bool MaterialInstance::ClearOverride(const String& name)
{
    return overrides.erase(name) != 0;
}

bool MaterialInstance::SetOverride(const String& name, MaterialParameterType expectedType, MaterialParameterValue value)
{
    if (!parent)
        return false;

    MaterialParameter parameter = {};
    if (!parent->TryGetParameter(name, parameter))
        return false;

    if (parameter.type != expectedType)
        return false;

    overrides.insert_or_assign(name, std::move(value));
    return true;
}

} // namespace URay
