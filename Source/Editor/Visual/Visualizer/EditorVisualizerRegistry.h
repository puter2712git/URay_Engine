#pragma once

#include <Core/Type/TypeAlias.h>
#include <Editor/Visual/Visualizer/EditorComponentVisualizer.h>

#include <functional>
#include <memory>
#include <unordered_map>

namespace URay
{

struct Type;
class Component;
class Unit;

class EditorVisualizerRegistry
{
public:
    using Constructor = std::function<UniquePtr<EditorComponentVisualizer>(EditorVisualContext&, Unit&, Component&)>;

    template <typename T>
    void Register(Constructor constructor)
    {
        constructors.insert_or_assign(&T::StaticType(), std::move(constructor));
    }

    const Constructor* Find(const Type* type) const
    {
        const auto it = constructors.find(type);
        return it != constructors.end() ? &it->second : nullptr;
    }

private:
    std::unordered_map<const Type*, Constructor> constructors;
};

} // namespace URay
