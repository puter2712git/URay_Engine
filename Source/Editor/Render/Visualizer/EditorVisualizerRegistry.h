#pragma once

#include "Editor/Render/Visualizer/EditorComponentVisualizer.h"

#include <functional>
#include <memory>
#include <unordered_map>

namespace URay
{

class Class;
class Component;
class Unit;

class EditorVisualizerRegistry
{
public:
    using Constructor = std::function<std::unique_ptr<EditorComponentVisualizer>(EditorVisualContext&, Unit&, Component&)>;

    template <typename T>
    void Register(Constructor constructor)
    {
        constructors.insert_or_assign(T::StaticClass(), std::move(constructor));
    }

    const Constructor* Find(Class* cls) const
    {
        const auto it = constructors.find(cls);
        return it != constructors.end() ? &it->second : nullptr;
    }

private:
    std::unordered_map<Class*, Constructor> constructors;
};

} // namespace URay
