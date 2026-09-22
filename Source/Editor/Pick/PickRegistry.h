#pragma once

#include "Editor/Pick/PickObject.h"

#include "Render/Rendering/Object/RenderObject.h"

#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace URay::Render
{
class RenderObject;
}

namespace URay
{

class Unit;

class PickRegistry
{
public:
    using Constructor = std::function<std::unique_ptr<PickObject>(Render::RenderObject&, Unit&)>;

    template <typename T>
    void Register(Constructor constructor)
    {
        constructors.insert_or_assign(std::type_index(typeid(T)), std::move(constructor));
    }

    const Constructor* Find(Render::RenderObject* object) const
    {
        const auto it = constructors.find(std::type_index(typeid(*object)));
        return it != constructors.end() ? &it->second : nullptr;
    }

private:
    std::unordered_map<std::type_index, Constructor> constructors;
};

} // namespace URay
