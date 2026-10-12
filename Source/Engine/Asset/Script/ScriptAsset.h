#pragma once

#include <Engine/Asset/Asset.h>
#include <Engine/Script/ScriptMetadata.h>

#include <string>
#include <vector>

namespace URay
{

class ScriptAsset final : public Asset
{
    URAY_CLASS(ScriptAsset, Asset)

public:
    const std::string& GetClassName() const { return className; }
    void SetClassName(std::string value) { className = std::move(value); }

    const std::vector<ScriptFieldDefinition>& GetFields() const { return fields; }
    void SetFields(std::vector<ScriptFieldDefinition> value) { fields = std::move(value); }

private:
    std::string className;
    std::vector<ScriptFieldDefinition> fields;
};

} // namespace URay
