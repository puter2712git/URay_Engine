#pragma once

#include "Editor/Render/EditorComponentVisualizer.h"

#include <memory>
#include <unordered_map>

namespace URay
{

class Class;

class EditorVisualizerRegistry
{
public:
    template <typename T>
    void Register(std::unique_ptr<EditorComponentVisualizer> visualizer)
    {
        visualizers.insert_or_assign(T::StaticClass(), std::move(visualizer));
    }

    EditorComponentVisualizer* Find(Class* cls) const
    {
        const auto it = visualizers.find(cls);
        return it != visualizers.end() ? it->second.get() : nullptr;
    }

private:
    std::unordered_map<Class*, std::unique_ptr<EditorComponentVisualizer>> visualizers;
};

} // namespace URay
