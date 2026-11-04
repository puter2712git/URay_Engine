#pragma once

#include <Core/Type/TypeAlias.h>
#include <Engine/Asset/Asset.h>
#include <Engine/Asset/Material/MaterialParameter.h>
#include <Engine/Asset/Material/MaterialRenderMode.h>

namespace URay
{

class Shader;

class MaterialInterface : public Asset
{
    URAY_OBJECT(MaterialInterface, Asset)

public:
    virtual ~MaterialInterface() = default;

public:
    virtual bool IsInitialized() const = 0;

    virtual Shader* GetShader() const = 0;

    virtual MaterialRenderMode GetRenderMode() const = 0;

    virtual bool TryGetParameter(const String& name, MaterialParameter& outParameter) const = 0;

    bool PrepareRenderMaterial(uint32 frameIndex);
};

} // namespace URay
