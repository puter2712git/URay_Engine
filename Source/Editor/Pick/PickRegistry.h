#pragma once

#include "Editor/Pick/PickObject.h"

#include "Render/Rendering/Object/RenderObject.h"

#include <functional>
#include <memory>
#include <unordered_map>

namespace URay::Render
{
class RenderObject;
}

namespace URay
{

class Unit;
struct RuntimeType;

class PickRegistry
{
public:
    using Constructor = std::function<std::unique_ptr<PickObject>(Render::RenderObject&, Unit&)>;

    template <typename T>
    void Register(Constructor constructor)
    {
        constructors.insert_or_assign(&T::StaticRuntimeType(), std::move(constructor));
    }

    const Constructor* Find(Render::RenderObject* object) const
    {
        const auto it = constructors.find(&object->GetRuntimeType());
        return it != constructors.end() ? &it->second : nullptr;
    }

private:
    std::unordered_map<const RuntimeType*, Constructor> constructors;
};

} // namespace URay
