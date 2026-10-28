#pragma once

#include <Core/Type/Type.h>
#include <Engine/Asset/AssetHandle.h>

#include <variant>

namespace URay
{

class Unit;

struct UnitInspectTarget
{
    Unit* unit = nullptr;
};

struct AssetInspectTarget
{
    AssetHandle handle = {};
};

using InspectTargetData = std::variant<
    std::monostate,
    UnitInspectTarget,
    AssetInspectTarget>;

struct InspectTarget
{
    URAY_PLAIN_CLASS(InspectTarget)

    InspectTargetData data = {};

    static InspectTarget FromUnit(Unit* unit)
    {
        return InspectTarget{
            .data = UnitInspectTarget{
                .unit = unit }
        };
    }

    static InspectTarget FromAsset(AssetHandle handle)
    {
        return InspectTarget{
            .data = AssetInspectTarget{
                .handle = handle }
        };
    }

    bool IsEmpty() const { return std::holds_alternative<std::monostate>(data); }
};

} // namespace URay
