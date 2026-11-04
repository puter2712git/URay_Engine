#pragma once

#include <Engine/Asset/Material/MaterialInterface.h>

namespace URay
{

class Material;

class MaterialInstance : public MaterialInterface
{
    URAY_OBJECT(MaterialInstance, MaterialInterface)

public:
    explicit MaterialInstance(const Material* parent);

public:
    const Material* GetParent() const;

    Shader* GetShader() const override;

    bool TryGetParameter(const String& name, MaterialParameter& outParameter) const override;

    bool SetFloat(const String& name, float value);
    bool SetFloat2(const String& name, const Vector2& value);
    bool SetFloat3(const String& name, const Vector3& value);
    bool SetFloat4(const String& name, const Color& value);
    bool SetTexture(const String& name, const AssetHandle& textureHandle);

    bool ClearOverride(const String& name);

private:
    bool SetOverride(const String& name, MaterialParameterType expectedType, MaterialParameterValue value);

private:
    const Material* parent = nullptr;

    HashMap<String, MaterialParameterValue> overrides;
};

} // namespace URay
