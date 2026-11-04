#pragma once

#include <Core/Type/TypeAlias.h>
#include <Engine/Asset/Asset.h>
#include <Engine/Asset/Material/MaterialParameter.h>

namespace URay
{

class Shader;

class MaterialInterface : public Asset
{
    URAY_OBJECT(MaterialInterface, Asset)

public:
    virtual ~MaterialInterface() = default;

public:
    virtual Shader* GetShader() const = 0;

    virtual bool TryGetParameter(const String& name, MaterialParameter& outParameter) const = 0;
};

} // namespace URay
